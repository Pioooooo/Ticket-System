from flask import Flask, request, render_template, session, redirect, url_for, abort
from executable import Executable, EndOfStream
from requests import post

app = Flask("web")
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'
if app.debug:
    app.jinja_env.auto_reload = True

root = 'root'
password = 'password'
core = Executable('../core/core.exe', root, password)


def __shutdown_server():
    func = request.environ.get('werkzeug.server.shutdown')
    if func is None:
        raise RuntimeError('Not running with the Werkzeug Server')
    func()


def __exit():
    global core
    try:
        del core
    except EndOfStream:
        pass
    __shutdown_server()


if app.debug:
    @app.before_request
    def test():
        print(str(core))


def __get_info():
    info = core.exec(['query_profile', '-c', session.get('username'), '-u', session.get('username')])[0].split(' ')
    ret = {'username': info[0], 'name': info[1], 'email': info[2], 'privilege': info[3]}
    return ret


@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'GET':
        if session.get('username') is None:
            return redirect(url_for('login', f='index'))
        else:
            return render_template('index.html', info=__get_info())
    else:
        if request.json['op'] == 0:
            ret = core.exec(
                ['query_transfer', '-s', request.json['from'], '-t', request.json['to'], '-d', request.json['date'],
                 '-p', request.json['sorting']])
            return {'e': 0, 'tot': int(ret[0]), 'result': ret[1:]}
        elif request.json['op'] == 1:
            ret = core.exec(
                ['query_ticket', '-s', request.json['from'], '-t', request.json['to'], '-d', request.json['date'], '-p',
                 request.json['sorting']])
            return {'e': 0, 'tot': int(ret[0]), 'result': ret[1:]}
        elif request.json['op'] == 2:
            if session.get('username') is None:
                return {'e': -1, 'msg': 'User is not logged in.'}
            ret = core.exec(
                ['buy_ticket', '-u', session.get('username'), '-i', request.json['trainID'], '-d', request.json['date'],
                 '-n', str(request.json['count']), '-f', request.json['from'], '-t', request.json['to'], '-q',
                 str(request.json['wait']).lower()])
            if ret[0] == '-1':
                return {'e': -1, 'msg': 'Buy ticket failed.'}
            else:
                return {'e': 0, 'tot': ret[0]}
        else:
            return {'e': -100, 'msg': 'Unrecognized request.'}


@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'GET':
        if session.get('username') is None:
            return render_template('login.html', f=request.args.get('f', 'index'))
        else:
            return redirect(url_for(request.args.get('f', 'index')))
    else:
        if request.json['op'] == 0:
            if request.json['username'] == root and request.json['password'] == password:
                session['username'] = 'root'
                return {'e': 0}
            ret = core.exec(
                ['login', '-u', request.json['username'], '-p', request.json['password']])
            if int(ret[0]):
                return {'e': -1}
            else:
                r = post('http://127.0.0.1:8000/check.php', {'username': request.json['username']}).json()
                if r['e'] == 0:
                    session['username'] = request.json['username']
                    return {'e': 0}
                else:
                    session['username2'] = request.json['username']
                    return {'e': 1}
        elif request.json['op'] == 1:
            ret = core.exec(['add_user', '-c', 'root', '-u', request.json['username'], '-p', request.json['password'],
                             '-n', request.json['name'], '-m', request.json['email'], '-g', '1'])
            return {'e': int(ret[0])}
        elif request.json['op'] == 2:
            if session.get('username') is None:
                return {'e': -1, 'msg': 'User is not logged in.'}
            session.pop('username')
            if session.get('username') != 'root':
                ret = core.exec(['logout', '-u', session.get('username')])
                return {'e': int(ret[0])}
            else:
                return {'e': 0}
        elif request.json['op'] == 3:
            r = post('http://127.0.0.1:8000/sendcode.php', {'phone': request.json['phone'], 'op': 1})
            return r.text
        elif request.json['op'] == 4:
            r = post('http://127.0.0.1:8000/login.php',
                     {'phone': request.json['phone'], 'code': request.json['code']}).json()
            if r['e'] == 0:
                session['username'] = r['username']
                return {'e': 0}
            else:
                return {'e': -1, 'msg': r['msg']}
        else:
            return {'e': -100, 'msg': 'Unrecognized request.'}


@app.route('/phoneverify', methods=['GET', 'POST'])
def phoneverify():
    if request.method == 'GET':
        if session.get('username2') is None:
            return redirect(url_for('index', f='index'))
        else:
            return render_template('phoneverify.html')
    else:
        if request.json['op'] == 0:
            r = post('http://127.0.0.1:8000/sendcode.php', {'phone': request.json['phone'], 'op': 0})
            return r.text
        elif request.json['op'] == 1:
            r = post('http://127.0.0.1:8000/phoneverify.php',
                     {'phone': request.json['phone'], 'code': request.json['code'],
                      'username': session['username2']}).json()
            if r['e'] == 0:
                session['username'] = session['username2']
                session.pop('username2')
                return {'e': 0}
            else:
                return {'e': -1, 'msg': r['msg']}
        else:
            return {'e': -100, 'msg': 'Unrecognized request.'}


@app.route('/manage', methods=['GET', 'POST'])
def manage():
    if request.method == 'GET':
        if session.get('username') is None:
            return redirect(url_for('login', f='manage'))
        else:
            return render_template('manage.html', info=__get_info())
    else:
        if request.json['op'] == 0:
            ret = core.exec(['query_profile', '-c', session['username'], '-u', request.json['username']])
            if ret[0] == '-1':
                return {'e': -1}
            return {'e': 0, 'result': ret[0]}
        elif request.json['op'] == 1:
            ret = core.exec(
                ['add_user', '-c', session['username'], '-u', request.json['username'], '-p', request.json['password'],
                 '-n', request.json['name'], '-m', request.json['email'], '-g', request.json['privilege']])
            return {'e': int(ret[0])}
        elif request.json['op'] == 2:
            args = ['modify_profile', '-c', session['username'], '-u', request.json['username']]
            args.extend(['-p', request.json['password']] if request.json['password'] != '' else [])
            args.extend(['-n', request.json['name']] if request.json['name'] != '' else [])
            args.extend(['-m', request.json['email']] if request.json['email'] != '' else [])
            args.extend(['-g', request.json['privilege']] if request.json['privilege'] != '' else [])
            ret = core.exec(args)
            if ret[0] == '-1':
                return {'e': -1}
            return {'e': 0, 'result': ret[0]}
        elif request.json['op'] == 3:
            ret = core.exec(['query_train', '-i', request.json['trainId'], '-d', request.json['saleDate'][:5]])
            if ret[0] == '-1':
                return {'e': -1}
            return {'e': 0, 'result': ret}
        elif request.json['op'] == 4:
            ret = core.exec(['add_train', '-i', request.json['trainId'], '-n', str(request.json['stationNumber']), '-m',
                             request.json['seatNumber'], '-s', request.json['stations'], '-p', request.json['prices'],
                             '-x', request.json['startTime'], '-t', request.json['travelTimes'], '-o',
                             request.json['stopoverTimes'], '-d', request.json['saleDate'], '-y',
                             request.json['trainType']])
            return {'e': int(ret[0])}
        elif request.json['op'] == 5:
            ret = core.exec(['delete_train', '-i', request.json['trainId']])
            return {'e': int(ret[0])}
        elif request.json['op'] == 6:
            ret = core.exec(['release_train', '-i', request.json['trainId']])
            return {'e': int(ret[0])}
        else:
            return {'e': -100, 'msg': 'Unrecognized request.'}


@app.route('/order', methods=['GET', 'POST'])
def order():
    if request.method == 'GET':
        if session.get('username') is None:
            return redirect(url_for('login', f='order'))
        else:
            return render_template('order.html', info=__get_info())
    else:
        if request.json['op'] == 0:
            ret = core.exec(['query_order', '-u', session['username']])
            if ret[0] == '-1':
                return {'e': -1}
            return {'e': 0, 'tot': int(ret[0]), 'result': ret[1:]}
        elif request.json['op'] == 1:
            ret = core.exec(['refund_ticket', '-n', request.json['n']])
            return {'e': int(ret[0])}
        else:
            return {'e': -100, 'msg': 'Unrecognized request.'}


@app.route('/account', methods=['GET', 'POST'])
def account():
    if request.method == 'GET':
        if session.get('username') is None:
            return redirect(url_for('login', f='account'))
        else:
            return render_template('account.html', info=__get_info())
    else:
        if request.json['op'] == 0:
            args = ['modify_profile', '-c', session['username'], '-u', session['username']]
            args.extend(['-p', request.json['password']] if request.json['password'] != '' else [])
            args.extend(['-n', request.json['name']] if request.json['name'] != '' else [])
            args.extend(['-m', request.json['email']] if request.json['email'] != '' else [])
            ret = core.exec(args)
            if ret[0] == '-1':
                return {'e': -1}
            return {'e': 0, 'result': ret[0]}
        else:
            return {'e': -100, 'msg': 'Unrecognized request.'}


@app.route('/exec', methods=['GET', 'POST'])
def execute():
    if request.method == 'GET':
        if session.get('username') == root:
            return render_template('execute.html', info=__get_info())
        abort(404)
    else:
        if session.get('username') == root:
            if request.json['op'] == 0:
                ret = []
                for args in request.json['cmd'].split('\n'):
                    ret.append(core.exec([args]))
                return {'result': ret}
            elif request.json['op'] == 1:
                __exit()
                return {'result': 'shutting down'}
        return {'result': 'request rejected'}


if app.debug:
    @app.route('/tmp')
    def tmp():
        return render_template('tmp.html', info=__get_info())

if __name__ == '__main__':
    app.run()
