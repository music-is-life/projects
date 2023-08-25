from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)

app.config["SQLALCHEMY_DATABASE_URI"] = "postgresql://postgres:postgres@db:5432/data"
db = SQLAlchemy(app)

#Create a model for Item table
class Item(db.Model):
    __tablename__ = 'Opened_Positions'

    id = db.Column(db.Integer, primary_key = True)
    company_name = db.Column(db.String(50), nullable = False)
    roll = db.Column(db.String(50), nullable=False)
    salary = db.Column(db.Float, nullable=True)

with app.app_context():
    db.create_all()

# Route to create a new item
@app.route("/items/", methods=["POST"])
def create_item():
    item = request.json
    new_item = Item(
                    company_name = item['company_name'],
                    roll = item['roll'],
                    salary = item['salary']
                    )
    db.session.add(new_item)
    db.session.commit()

    return jsonify(
                    {"id": new_item.id,
                    "company_name": new_item.company_name,
                    "roll": new_item.roll,
                    "salary": new_item.salary
                    }
                  ), 200
# Route to get a list of all items
@app.route("/items/", methods=["GET"])
def get_items():
    result = Item.query.all()
    items = []
    for item in result:
        items.append(
                        {
                            "id": item.id,
                            "company_name": item.company_name,
                            "roll": item.roll,
                            "salary": item.salary
                        }
                    )
    return jsonify(items), 200

# Route to get a specific item by its ID
@app.route("/items/<int:id>", methods=["GET"])
def get_item(id):
    result = Item.query.get(id)
    if result is not None:
            return jsonify(
                            {"id": result.id,
                            "company_name": result.company_name,
                            "roll": result.roll,
                            "salary": result.salary
                            }
                          ), 200    
    return jsonify({"message": "Item not found"}), 404

# Route to update an existing item by its ID
@app.route("/items/<int:id>", methods=["PUT"])
def update_item(id):
    result = Item.query.get(id)
    if result:
        updated_item = request.json
        result.company_name = updated_item["company_name"]
        result.roll = updated_item["roll"]
        result.salary = updated_item["salary"]
        db.session.commit()
        return jsonify(
                        {"id": result.id,
                        "company_name": result.company_name,
                        "roll": result.roll,
                        "salary": result.salary
                        }
                      ), 200    
    return jsonify({"message": "Item not found"}), 404

# Route to delete an item by its ID
@app.route("/items/<int:id>", methods=["DELETE"])
def delete_item(id):
    result = Item.query.get(id)
    if result:
            db.session.delete(result)
            db.session.commit()
            return jsonify(
                            {"id": result.id,
                            "company_name": result.company_name,
                            "roll": result.roll,
                            "salary": result.salary
                            }
                          ), 200
    return jsonify({"message": "Item not found"}), 404

if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True)