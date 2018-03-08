from tornado import websocket, web, ioloop
import json
import os

cl = []


class IndexHandler(web.RequestHandler):
    def get(self):
        self.render("index.html")


class SocketHandler(websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    def open(self):
        if self not in cl:
            cl.append(self)

    def on_message(self, message):
        print "receive " + message
        send_message = {}
        send_message['data'] = message
        for conn in cl:
            conn.write_message(send_message)
        # if conn is not self:

    def on_close(self):
        if self in cl:
            cl.remove(self)


class ApiHandler(web.RequestHandler):

    @web.asynchronous
    def get(self, *args):
        self.finish()
        id = self.get_argument("id")
        value = self.get_argument("value")
        data = {"id": id, "value": value}
        data = json.dumps(data)
        for c in cl:
            c.write_message(data)

    @web.asynchronous
    def post(self):
        pass


app = web.Application([
    (r'/', IndexHandler),
    (r'/ws', SocketHandler),
    (r'/api', ApiHandler),
    (r'/(.*)', web.StaticFileHandler, {'path': os.getcwd()})
])


if __name__ == '__main__':
    print "server listening on port 8888"
    app.listen(8888)
    ioloop.IOLoop.instance().start()
