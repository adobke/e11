import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import threading,time,serial
 
clients = []
    
class Game():
  def __init__(self):
    self.bonus = "none"
    self.beacons = { 1: "white", 2: "green", 3: "white", 4: "green", 5: "green", 6: "white", 7: "green", 8: "white", 9: "none" }
    self.state = "prematch"

  def getScores(self):
    white = 0
    green = 0
    if self.bonus == "white": white += 1
    if self.bonus == "green": green += 1
    
    for x in range(1,7) + [9]:
      if self.beacons[x] == "white":
        white += 1
      elif self.beacons[x] == "green":
        green += 1

    for x in [7,8]:
      if self.beacons[x] == "white":
        white += 2
      elif self.beacons[x] == "green":
        green += 2

    return [white,green]

game = Game()
 
class ClientHandler(tornado.websocket.WebSocketHandler):
    def open(self):
      print 'new connection'
      clients.append(self)
      
    def on_message(self, message):
      print 'message received %s' % message
 
    def on_close(self):
      print 'connection closed'
      clients.remove(self)
 
class EditHandler(tornado.websocket.WebSocketHandler):
    def open(self):
      print 'new connection'
      clients.append(self)
      
    def on_message(self, message):
      global game
      print 'message received %s' % message
      if message == "startmatch":
        game.state == "inmatch"
        for client in clients:
          client.write_message("startmatch")
      elif message == "reset":
        game.state == "prematch"
        for client in clients:
          client.write_message("reset")
        game = Game()
        scores = game.getScores()
        print scores
        for client in clients:
          client.write_message("scores " + str(scores[0]) + " " \
            + str(scores[1]))

      elif message == "endmatch":
        game.state = "postmatch"
        for client in clients:
          client.write_message("pause")

      elif message.split()[0] == "beacon":
        num = int(message.split()[1])
        team = message.split()[2]
        game.beacons[num] = team
        scores = game.getScores()
        for client in clients:
          client.write_message("scores " + str(scores[0]) + " " \
            + str(scores[1]))

      elif message.split()[0] == "bonus":
        team = message.split()[1]
        game.bonus = team
        for client in clients:
          client.write_message("bonus " + team)
        scores = game.getScores()
        for client in clients:
          client.write_message("scores " + str(scores[0]) + " " \
            + str(scores[1]))

    def on_close(self):
      print 'connection closed'
      clients.remove(self)
 
application = tornado.web.Application([
    (r'/ws', ClientHandler),
    (r'/wsEdit', EditHandler),
])

class ScoreBoardManager(threading.Thread):
  def __init__(self,port):
    self.aComm = serial.Serial(port, baudrate=115200)
    self.lastState = None
    threading.Thread.__init__(self)

  def run(self):
    global game
    print "running.."
    while True:
      if (game.state != self.lastState):
        self.stateUpdate(game.state)
      self.scoreUpdate()
      delay(.2)
   
  def stateUpdate(self, state):
    print "sending state"
    msg = "t"
    if state == "prematch":
      msg += "p"
    elif state == "inmatch":
      msg += "i"
    elif state == "postmatch":
      msg += "e"
    else:
      print "state sending error"
      return
    self.writeStr(msg)

  def scoreUpdate(self):
    global game
    print "sending score "
    msg = ""
    msg += 's'
    scores = game.getScores()
    msg += chr(scores[0])
    msg += chr(scores[1])
    self.writeStr(msg)

  def writeStr(self,msg):
    msg += '\x00'
    self.aComm.write(msg)


if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(8888)
    #x = ScoreBoardManager("/dev/ttyUSB0")
    #x.start()
    print "past..."
    tornado.ioloop.IOLoop.instance().start()

