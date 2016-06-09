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
import play.api.libs.json._


/**
 * This controller creates an `Action` to handle HTTP requests to the
 * application's home page.
 */
@Singleton
class HomeController @Inject() (implicit system: ActorSystem, materializer: Materializer) extends Controller {

  /* Main Page that will print the graphs */
  def index = Action { implicit request =>
    Ok(views.html.main("MAIN", HomeController.names.toList,
      request.session.get("connected") != None))
  }

  /* Adds a new measure in the database */
  def addNewMeasure(id : String, data : Double, currentTime : java.util.Date) = {
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
    val measure = getMeasures(id).takeRight(50)
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

  /* Check if the sensor was read before */
  def isSensor(id : String) = {
    this.synchronized {
      HomeController.ids contains id
    }
  }

  /* Receives a measure from a sensor */
  def measureData(id : String, dataType : String, data : Float, time : Long) = Action {
    val currentTime = new java.util.Date(time)
    var temp = 0.0

    /* Here we can do some conversion we did not do on board */
    if (dataType contains "TEMP"){
        if (data != 65532){
          temp = -46.85 + 175.72 * data / 65536.0
          addNewMeasure(id + dataType, temp, currentTime)
        }
    } else if (dataType contains "HUMI") {
        if (data != 65532){
          temp = -6.0 + 125 * data / 65536.0
          addNewMeasure(id + dataType, temp, currentTime)
        }
    } else {
        /* Default data, do nothing special */
        temp = data
        addNewMeasure(id + dataType, data, currentTime)
    }

    setClass(id + dataType, dataType)
    setStatus(id + dataType, SensorState.Active)
    addNewSensor(id + dataType)

    /* Send the update to connected users */
    for (actor <- MyWebSocketActor.getActors(id + dataType)){
      if (actor != null) {
        actor ! (currentTime.getTime() + " " + temp)
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

  /* Handles the web socket creation */
  def socket(id : String) = WebSocket.accept[String, String] { request =>
    ActorFlow.actorRef(out => MyWebSocketActor.props(out, id))
  }

  /* Send the data chart for a given id */
  def chart(id : String) = Action { implicit request =>
    if (isSensor(id)) {
      Ok(views.html.chart(id, getName(id), getClass(id), toReadableData(id)))
    } else {
      NoContent
    }
  }

  /* Simulation of sensor */
  def sensor = Action {
    Ok(views.html.sensor())
  }

  /* Form to change name sensor */
  val SensorParamsForm = Form(
    mapping(
      "id" -> text,
      "name" -> text
    )(SensorParams.apply)(SensorParams.unapply))

  def toSensorParams(ids : Set[String]) = {
      ids.map(id => SensorParams(id, getName(id)))
  }


  /* A page to modify the parameters of a sensor */
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

  val IdForm = Form(
      mapping(
          "id" -> text,
          "password" -> text
      )(Id.apply)(Id.unapply))

  def log = Action { implicit request =>
      IdForm.bindFromRequest.fold(
          formWithErrors => {
              BadRequest("Problem with login.")
          },
          id => {
              if (id.id == "admin" && id.password == "admin")
                  Redirect(routes.HomeController.index).withSession(
                      "connected" -> "admin")
              else
                  Ok("Bad Id or Password")
          }
      )
  }

  def login = Action { implicit request =>
      request.session.get("connected").map { user =>
          Redirect(routes.HomeController.index)
      }.getOrElse(
          Ok(views.html.login(IdForm))
      )
  }

  def disconnect = Action { implicit request =>
      Redirect(routes.HomeController.index).withSession(
          request.session - "connected")
  }

  /* The page for the sensor modification */
  def params = Action { implicit request =>
      request.session.get("connected").map { user =>
          Ok(views.html.params(toSensorParams(HomeController.ids),
              SensorParamsForm, HomeController.accThd))
      }.getOrElse{
          Redirect(routes.HomeController.index)
      }
  }

  /* Get the status of a sensor */
  def getStatus(id : String) = {
    this.synchronized {
      HomeController.status.getOrElse(id, SensorState.Disconnected)
    }
  }

  /* Set the status of a sensor */
  def setStatus(id : String, status : SensorState.Value) = {
    this.synchronized {
      HomeController.status += (id -> status)
    }
  }

  /* Get the list of all sensors */
  def getSensors = {
    this.synchronized {
      HomeController.ids.toList
    }
  }

  /* Create a Json to send the list of sensors */
  def listSensors = Action { implicit request =>
    Ok(Json.toJson(getSensors.map((id : String) => Json.obj(
      "id" -> id,
      "name" -> getName(id),
      "status" -> getStatus(id).toString
    ))))
  }

  /* When the list of ids is received */
  def updateSensors(ids : String) = Action {
    /* The list is sent everything packed in one string */
    for(id <- getSensors){
      if (ids contains id.substring(0, id.length() - 4)) {
        setStatus(id, SensorState.Active)
      } else {
        setStatus(id, SensorState.Disconnected)
      }
    }
    Ok("")
  }

  /* Set the accelerometer value */
  def setAccThd(thd : Float) = Action {
    this.synchronized{
      HomeController.accThd = thd
    }
    Ok("")
  }

  /* Returns the Json for parameters */
  def getParameters = Action {
    val json : JsValue = JsObject(Seq(
      "accthd" -> JsNumber(HomeController.accThd)
    ))
    Ok(json)
  }

}

/* Describe the state of a sensor */
object SensorState extends Enumeration {
  val Active, Disconnected, Malfunction = Value
}

/* Parameters of sensor, for the form modification */
case class SensorParams(id : String, name : String)

/* Contains a measure */
case class Measure(date : java.util.Date, measure : Double)

/* Contains information about a sensor */
case class SensorInfo(id : String, name : String, status : SensorState.Value)

case class Id(id : String, password : String)

object HomeController {
  /* Some global variables , they need to be changed to database accesses */
  var measures = Map[String, List[Measure]]()
  var ids = Set[String]()
  var classes = Map[String, String]()
  var names = Map[String, String]()
  var status = Map[String, SensorState.Value]()
  var accThd : Float = 0;
}
