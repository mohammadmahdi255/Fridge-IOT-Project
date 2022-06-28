#!/usr/bin/python3
import json
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
            # Increase temp
            # Send Request to end point
            #     url = 'http://exemple.com'
            # headers = {'Content-type': 'text/html; charset=UTF-8'}
            # response = requests.post(url, data=data, headers=headers)
            object = Action(assignee='IOT-Manager', action='Increase Temprature', status='Done')
            db.session.add(object)
            db.session.commit()
        elif action_type == 2:
            # Decrease temp
            object = Action(assignee='IOT-Manager', action='Decrease Temprature', status='Done')
            db.session.add(object)
            db.session.commit()
            pass
        elif action_type == 3:
            # Mute
            object = Action(assignee='IOT-Manager', action='Mute', status='Done')
            db.session.add(object)
            db.session.commit()
        elif action_type == 4:
            # Super cooling
            object = Action(assignee='IOT-Manager', action='Super cooling', status='Done')
            db.session.add(object)
            db.session.commit()
        elif action_type == 5:
            # Power off/on
            object = Action(assignee='IOT-Manager', action='Power Off/On', status='Done')
            db.session.add(object)
            db.session.commit()
        else:
            return {'status':'wrong'}

        return {'status':'success'}

api.add_resource(Notes, '/notes')
api.add_resource(Actions, '/actions')

if __name__ == '__main__':
     app.run()
