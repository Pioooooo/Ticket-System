# 前端 #

主体部分采用flask框架，短信验证部分使用php配合mysql进行编写。

## Flask部分 ##

- `app.py`
  - 控制所有页面的路由
  - 提供除短信登录部分的api接口
  - 通过标准控制台输入输出与后端程序进行交互
  - 配置初始 `root` 与其 `password`
  - 进行 `session` 的管理
- `executable.py`
  - `Class Executable`
    - `__init__(self, s, root='root', password='password')`
      - 打开后端程序
      - 添加 `root` 用户
    - `__del__(self)`
      - 关闭后端程序
    - `exec(self, s, timeout=0.1)`
      - 以 `timeout` 为等候延迟执行参数列表 `s` 中的命令
      - 以列表的形式返回后端程序的输出（每一个元素为输出的一行）
      - 若后端程序在 `timeout` 时间内没有输出则抛出异常 `EndOfStream`
  - `Class EndOfStream(Exception)`
    - `pass`

## 具体路由 ##

- `/login`
  - `GET`
    - 登陆及注册页面
    - `GET` 参数 `f` ：登录成功后重定向路由
  - `POST`
    - `json['op'] == 0`
      - 账号密码登录
    - `json['op'] == 1`
      - 注册
    - `json['op'] == 2`
      - 登出
    - `json['op'] == 3`
      - 发送验证码
    - `json['op'] == 4`
      - 短信登录

- `/`
  - 必须已登录
  - 未登录跳转至 `/login`
  - `GET`
    - 查询车票页面
  - `POST`
    - `json['op'] == 0`
      - 查询中转
    - `json['op'] == 1`
      - 查询车票
    - `json['op'] == 2`
      - 购买车票

- `/phoneverify`
  - `GET`
    - 绑定手机页面
  - `POST`
    - `json['op'] == 0`
      - 发送验证码
    - `json['op'] == 1`
      - 提交验证码

- `/manage`
  - 必须已登录
  - 未登录跳转至 `/login`
  - `GET`
    - 后台管理页面
  - `POST`
    - `json['op'] == 0`
      - 查询用户信息
    - `json['op'] == 1`
      - 添加用户
    - `json['op'] == 2`
      - 修改用户信息
    - `json['op'] == 3`
      - 查询列车信息
    - `json['op'] == 4`
      - 添加列车
    - `json['op'] == 5`
      - 删除列车
    - `json['op'] == 6`
      - 发布列车

- `/order`
  - 必须已登录
  - 未登录跳转至 `/login`
  - `GET`
    - 订单页面
  - `POST`
    - `json['op'] == 0`
      - 查询订单信息
    - `json['op'] == 1`
      - 退票

- `/account`
  - 必须已登录
  - 未登录跳转至 `/login`
  - `GET`
    - 个人中心页面
  - `POST`
    - `json['op'] == 0`
      - 修改个人信息

- `/exec`
  - 必须登录为 `root` 账户
  - 非 `root` 账户返回 `404 Not Found`
  - `GET`
    - 批量指令执行页面
  - `POST`
    - `json['op'] == 0`
      - 批量执行指令
    - `json['op'] == 1`
      - 关闭服务器


- `/tmp`
  - 调试模式下存在
  - `GET`
    - 返回 `tmp.html`

## HTML杂项 ##

- `base.html`
  - 除 `login.html` 与 `phoneverify.html` 外所有html的模板
  - 引用 `bootstrap`、 `jquery`、 `feather`
  - 绘制了导航栏、底部页脚
  - 添加公共 `js` 脚本
  - 设定主要内容 `style`

