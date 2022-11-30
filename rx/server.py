from flask import Flask
import json
app = Flask(__name__)


@app.route('/')
def hello_world():
   return 'Hello World'

@app.route("/getdata", methods=["GET","POST"])
def upvote():
    return json.dumps({'status':'nol'})

if __name__ == '__main__':
   app.run() 
