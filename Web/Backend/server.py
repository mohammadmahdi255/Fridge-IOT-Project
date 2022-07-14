#!/usr/bin/python3
import json
import requests
from flask import Flask, request, jsonify
from flask_restful import Resource, Api
from json import dumps
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy.inspection import inspect
from flask_cors import CORS, cross_origin


app = Flask(__name__)
CORS(app)
api = Api(app)
db = SQLAlchemy(app)
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///project.db"
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
url = "http://192.168.137.111"

def convertToJson(content):
    fixed_string = str(content)[4:-7]
    return json.loads(fixed_string)

class Note(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    text = db.Column(db.String(50))
    done = db.Column(db.Boolean)
    @property
    def serialize(self):
       return {
           'id': self.id,
           'text': self.text,
           'done'  : self.done
       }
    @property
    def serialize_many2many(self):
       return [ item.serialize for item in self.many2many]

class Action(db.Model):
    id = db.Column(db.Integer, primary_key = True)
    assignee = db.Column(db.String(50))
    action = db.Column(db.String(50))
    status = db.Column(db.String(50))
    @property
    def serialize(self):
       return {
           'id': self.id,
           'assignee': self.assignee,
           'action'  : self.action,
           'status': self.status
       }
    @property
    def serialize_many2many(self):
       return [ item.serialize for item in self.many2many]

db.create_all()

class Notes(Resource):
    def get(self):
        query_result = Note.query.all()
        return jsonify(notes=[i.serialize for i in query_result])
    def post(self):
        print(request.json)
        text = request.json['text']
        done = request.json['done']
        object = Note(text=text, done=False)
        db.session.add(object)
        db.session.commit()
        return {'status':'success', 'text': text, 'done': done}

class Actions(Resource):
    def get(self):
        query_result = Action.query.all()
        return jsonify(actions=[i.serialize for i in query_result])
    def post(self):
        action_type = request.json['type']
        if action_type == 1:
            response = requests.request("GET", url + '/1/', headers={}, data={})
            data = convertToJson(response.content)
            if data['message'] == "successfull":
                object = Action(assignee='IOT-Manager', action='Increase Temprature', status='Done')
                db.session.add(object)
                db.session.commit()
        elif action_type == 2:
            # Decrease temp
            response = requests.request("GET", url + '/2/', headers={}, data={})
            data = convertToJson(response.content)
            if data['message'] == "successfull":
                object = Action(assignee='IOT-Manager', action='Decrease Temprature', status='Done')
                db.session.add(object)
                db.session.commit()
        elif action_type == 3:
            # Mute
            response = requests.request("GET", url + '/3/', headers={}, data={})
            data = convertToJson(response.content)
            if data['message'] == "successfull":
                object = Action(assignee='IOT-Manager', action='Mute', status='Done')
                db.session.add(object)
                db.session.commit()
        elif action_type == 4:
            # Super cooling
            response = requests.request("GET", url + '/4/', headers={}, data={})
            data = convertToJson(response.content)
            if data['message'] == "successfull":
                object = Action(assignee='IOT-Manager', action='Super cooling', status='Done')
                db.session.add(object)
                db.session.commit()
        elif action_type == 5:
            response = requests.request("GET", url + '/5/', headers={}, data={})
            data = convertToJson(response.content)
            if data['message'] == "successfull":
            # Power off/on
                object = Action(assignee='IOT-Manager', action='Power Off/On', status='Done')
                db.session.add(object)
                db.session.commit()
        else:
            return {'status':'wrong'}

        return {'status':'success'}

class Info(Resource):
    def get(self):
        response = requests.request("GET", url + '/info/', headers={}, data={})
        data = convertToJson(response.content)
        print(data)
        return {"current_temp": data["current_temp"], "request_temp": data["request_temp"], "current_status": data["current_status"]}

api.add_resource(Notes, '/notes')
api.add_resource(Actions, '/actions')
api.add_resource(Info, '/info')

if __name__ == '__main__':
     app.run()
