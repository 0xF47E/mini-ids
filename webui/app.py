from flask import Flask, render_template, jsonify

app = Flask(__name__)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/scans")
def get_scans():
    from storage import datastorage

    scan_id = datastorage.get_lastest_scan_id()
    if scan_id:
        results = datastorage.get_latest_results(scan_id)
        return jsonify({"scan_id": scan_id, "results": results})
    return jsonify({"results": []})
