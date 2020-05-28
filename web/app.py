from flask import Flask, request, render_template, session, redirect, url_for
from executable import Executable

app = Flask("webq")
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'
app.jinja_env.auto_reload = True

core = Executable('../core/core.exe')


@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'GET':
        if session.get('username') is None:
            return redirect(url_for('login', f='index'))
        else:
            return render_template('index.html', ses=session)
    else:
        if request.json['transit']:
            cmd = ['query_transfer']
        else:
            cmd = ['query_ticket']
        ret = core.exec(cmd + ['-s', request.json['from'], '-t', request.json['to'], '-d', request.json['date']])
        return {'e': 0, 'tot': int(ret[0]), 'result': ret[1:]}


@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'GET':
        if session.get('username') is None:
            return render_template('login.html', f=request.args.get('f', 'index'))
        else:
            return redirect(url_for(request.args.get('f', 'index')))
    else:
        if request.json['type'] == 'login':
            ret = core.exec(['login', '-u', request.json['username'], '-p', request.json['password']])
            if int(ret[0]):
                return {'e': -1}
            else:
                session['username'] = request.json['username']
                return {'e': 0}
        elif request.json['type'] == 'register':
            ret = core.exec(['add_user', '-c', 'root', '-u', request.json['username'], '-p', request.json['password'],
                             '-n', request.json['name'], '-m', request.json['email'], '-g', '1'])
            return {'e': int(ret[0])}


@app.route('/manage')
def manage():
    if session.get('username') is None:
        return redirect(url_for('login', f='manage'))
    else:
        return render_template('manage.html', ses=session)


@app.route('/exec', methods=['POST'])
def execute():
    data = request.json
    if data:
        tmp = core.exec([data['cmd']])
        return {'result': tmp}


@app.route('/tmp')
def tmp():
    return render_template('tmp.html', f="index")


if __name__ == '__main__':
    app.run()
