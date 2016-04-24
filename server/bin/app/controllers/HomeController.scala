package controllers

import javax.inject._
import play.api._
import play.api.mvc._
import play.api.libs.streams._
import akka.actor._
import akka.stream._
import akka.stream.scaladsl._
import scala.concurrent.Future
import math._
import play.api.data._
import play.api.data.Forms._
import play.api.Play.current
import play.api.i18n.Messages.Implicits._
import play.api.libs.json.{JsNull,Json,JsString,JsValue}


/**
 * This controller creates an `Action` to handle HTTP requests to the
 * application's home page.
 */
@Singleton
class HomeController @Inject() (implicit system: ActorSystem, materializer: Materializer) extends Controller {

  /* Main Page that will print the graphs */
  def index = Action {
    Ok(views.html.main("MAIN", HomeController.names.toList))
  }

  /* Adds a new measure in the database */
  def addNewMeasure(id : String, data : Float, currentTime : java.util.Date) = {
    this.synchronized{
      HomeController.measures += (id -> (Measure(currentTime, data) :: HomeController.measures.getOrElse(id, List[Measure]())))

    }
  }

  /* Get the list of measured data */
  def getMeasures(id : String) = {
    HomeController.measures.getOrElse(id, List()).reverse
  }

  /* Convert a list of measures to data for the chart */
  def toReadableData(id : String) = {
    val measure = getMeasures(id)
    var result = List[String]()
    for (m <- measure) {
      result = "[" :: result
      result = m.date.getTime.toString :: result
      result = "," :: result
      result = m.measure.toString :: result
      result = "]" :: result
      result = "," :: result
    }
    if (!measure.isEmpty)
        result.tail.reverse.mkString
    else ""
  }

  /* Add a new sensor to the set of sensors */
  def addNewSensor(id : String) = {
    this.synchronized{
      HomeController.ids += id
    }
  }

  /* Sets the type of data a sensor is reading */
  def setClass(id : String, dataType : String) = {
    this.synchronized{
      HomeController.classes += (id -> dataType)
    }
  }

  /* Get the type of data */
  def getClass(id : String) = {
    this.synchronized{
      HomeController.classes.getOrElse(id, "NOTYPE")
    }
  }

  def isSensor(id : String) = {
    this.synchronized {
      HomeController.ids contains id
    }
  }

  /* Receives a measure from a sensor */
  def measureData(id : String, dataType : String, data : Float) = Action {
    val currentTime = java.util.Calendar.getInstance.getTime

    addNewMeasure(id, data, currentTime)
    setClass(id, dataType)
    setStatus(id, SensorState.Active)
    addNewSensor(id)

    /* Send the update to connected users */
    for (actor <- MyWebSocketActor.getActors(id)){
      if (actor != null) {
        actor ! (currentTime.getTime() + " " + data)
      }
    }

    Ok("You are " + id + " and you measured " + data).withHeaders(
      ACCESS_CONTROL_ALLOW_ORIGIN -> "*")
  }

  /* Change the name of a sensor to make it easier to recognize it */
  def setName(id : String, name : String) = {
    this.synchronized {
      HomeController.names += (id -> name)
    }
  }

  /* Get the name of a sensor */
  def getName(id : String) = {
    this.synchronized {
      HomeController.names.getOrElse(id, id)
    }
  }

  def socket(id : String) = WebSocket.accept[String, String] { request =>
    ActorFlow.actorRef(out => MyWebSocketActor.props(out, id))
  }

  def chart(id : String) = Action { implicit request =>
    if (isSensor(id)) {
      Ok(views.html.chart(id, getName(id), getClass(id), toReadableData(id)))
    } else {
      NoContent
    }
  }

  def sensor = Action {
    Ok(views.html.sensor())
  }

  val SensorParamsForm = Form(
    mapping(
      "id" -> text,
      "name" -> text
    )(SensorParams.apply)(SensorParams.unapply))


  def modifyParams = Action { implicit request =>
    SensorParamsForm.bindFromRequest.fold(
      formWithErrors => {
        BadRequest("Check data")
      },
      sensor => {
        setName(sensor.id, sensor.name)
        Redirect(routes.HomeController.index)
      }
    )
  }

  def params = Action { implicit request =>
    var result = Ok("Nothing to change")
    var SensorParamsFormFill : Form[SensorParams] = null
    for(id <- HomeController.ids){
      val name = getName(id) 
      if (name == id){
        SensorParamsFormFill = SensorParamsForm.fill(SensorParams(id,""))
        result = Ok(views.html.params(id, SensorParamsFormFill))
      }
    }
    result
  }

  def getStatus(id : String) = {
    this.synchronized {
      HomeController.status.getOrElse(id, SensorState.Disconnected)
    }
  }

  def setStatus(id : String, status : SensorState.Value) = {
    this.synchronized {
      HomeController.status += (id -> status)
    }
  }

  def getSensors = {
    this.synchronized {
      HomeController.ids.toList
    }
  }

  def listSensors = Action { implicit request =>
    Ok(Json.toJson(getSensors.map((id : String) => Json.obj(
      "id" -> id,
      "name" -> getName(id),
      "status" -> getStatus(id).toString
    ))))
  }

  def updateSensors(ids : String) = Action {
    for(id <- getSensors){
      if (ids contains id) {
        setStatus(id, SensorState.Active)
      } else {
        setStatus(id, SensorState.Disconnected)
      }
    }
    Ok("")
  }

}

object SensorState extends Enumeration {
  val Active, Disconnected, Malfunction = Value
}

case class SensorParams(id : String, name : String)

case class Measure(date : java.util.Date, measure : Float)
case class SensorInfo(id : String, name : String, status : SensorState.Value)

object HomeController {
  var measures = Map[String, List[Measure]]()
  var ids = Set[String]()
  var classes = Map[String, String]()
  var names = Map[String, String]()
  var status = Map[String, SensorState.Value]()
}
