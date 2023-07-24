from flask import Flask, request, jsonify

app = Flask(__name__)

# File to store data (JSON format)
data_file = "data.json"

# Helper function to read data from the file
def read_data():
    try:
        with open(data_file, "r") as file:
            data = file.read()
            if data:
                return eval(data)  # Using eval is not recommended in production. For simplicity, we use it here.
            return []
    except FileNotFoundError:
        return []

# Helper function to write data to the file
def write_data(data):
    with open(data_file, "w") as file:
        file.write(str(data))

# Route to create a new item
@app.route("/items/", methods=["POST"])
def create_item():
    data = read_data()
    item = request.json
    data.append(item)
    write_data(data)
    return jsonify(item), 201

# Route to get a list of all items
@app.route("/items/", methods=["GET"])
def get_items():
    data = read_data()
    return jsonify(data), 200

# Route to get a specific item by its ID
@app.route("/items/<int:item_id>", methods=["GET"])
def get_item(item_id):
    data = read_data()
    if 0 <= item_id < len(data):
        return jsonify(data[item_id]), 200
    return jsonify({"message": "Item not found"}), 404

# Route to update an existing item by its ID
@app.route("/items/<int:item_id>", methods=["PUT"])
def update_item(item_id):
    data = read_data()
    if 0 <= item_id < len(data):
        data[item_id] = request.json
        write_data(data)
        return jsonify(data[item_id]), 200
    return jsonify({"message": "Item not found"}), 404

# Route to delete an item by its ID
@app.route("/items/<int:item_id>", methods=["DELETE"])
def delete_item(item_id):
    data = read_data()
    if 0 <= item_id < len(data):
        deleted_item = data.pop(item_id)
        write_data(data)
        return jsonify(deleted_item), 200
    return jsonify({"message": "Item not found"}), 404

if __name__ == "__main__":
    app.run(debug=True)