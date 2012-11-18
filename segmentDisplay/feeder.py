import websocket
import thread
import time,serial

class ScoreBoardManager():
  def __init__(self,port):
    self.aComm = serial.Serial(port, baudrate=115200)
    self.lastState = None
   
  def stateUpdate(self, state):
    if (state == self.lastState):
      return
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

  def scoreUpdate(self,scores):
    msg = ""
    msg += 's'
    msg += chr(scores[0])
    msg += chr(scores[1])
    self.writeStr(msg)

  def writeStr(self,msg):
    msg += '\x42'
    self.aComm.write(msg)



def on_message(ws, message):
  global sb
  cmd = message.split()[0]

  if cmd == "scores":
    white = int(message.split()[1])
    green = int(message.split()[2])
    sb.scoreUpdate([white,green])
  elif cmd == "startmatch":
    sb.stateUpdate("inmatch")
  elif cmd == "reset":
    sb.stateUpdate("prematch")
  elif cmd == "pause":
    sb.stateUpdate("postmatch")



def on_error(ws, error):
    print error

def on_close(ws):
    print "### closed ###"

def on_open(ws):
    def run(*args):
        while(True):
          time.sleep(5)
        ws.close()
        print "thread terminating..."
    thread.start_new_thread(run, ())


if __name__ == "__main__":
    global sb
    sb = ScoreBoardManager("/dev/ttyUSB0")
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("ws://belka.st.hmc.edu:8888/wsEdit",
                                on_message = on_message,
                                on_error = on_error,
                                on_close = on_close)
    ws.on_open = on_open

    ws.run_forever()
