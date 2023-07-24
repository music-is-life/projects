from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello WOrldddddd'

@app.route('/home/<name>')
def type_bullshit(name):
    return f"Whatsup {name} ?? stop lying to usssss"

if __name__ == '__main__':
    app.run(debug=True)