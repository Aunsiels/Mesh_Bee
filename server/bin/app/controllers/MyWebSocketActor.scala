package controllers

import akka.actor._
import play.api.libs.json.JsValue

object MyWebSocketActor {
  var mapping = Map[String, List[ActorRef]]()

  def props(out: ActorRef, id : String)(implicit factory: ActorRefFactory) = {
    var temp  = Props(new MyWebSocketActor(out))
    this.synchronized{
      mapping += (id -> (factory.actorOf(temp) :: mapping.getOrElse(id, List())))
    }
    temp
  }

  def getActors(id : String) = {
    this.synchronized{
      mapping.getOrElse(id, List())
    }
  }
}

class MyWebSocketActor(out: ActorRef) extends Actor {
  def receive = {
    case msg: String =>
      out ! msg
  }
}
