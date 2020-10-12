from flask import Flask, make_response

app = Flask("alt-svc tester")

@app.route('/')
def default():
    res = make_response('not safe')
    res.headers['alt-svc'] = 'h2=:8888; ma=65535'
    return res

if __name__ == "__main__":
    app.run(port=5000, ssl_context='adhoc')
