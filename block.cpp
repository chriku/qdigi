#include "block.h"
#include "painter.h"
#include "settings.h"
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QTime>
extern "C" {
#include "luasocket.h"
}

Block::Block(QObject *parent) : QObject(parent) { L = luaL_newstate(); }

void Block::load(QString fileName) {
  Block::fileName = fileName;
  QFile file(fileName);
  file.open(QFile::ReadOnly);
  QString data = file.readAll();
  luaL_openlibs(L);
  // luaopen_socket_core(L);
  // lua_setglobal(L,"socket");
  if (luaL_loadstring(L, data.toUtf8().data()) == LUA_OK) {
    if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
      lua_getfield(L, -1, "pins");
      lua_pushnil(L);
      while (lua_next(L, -2)) {
        lua_geti(L, -1, 1);
        int x = lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_geti(L, -1, 2);
        int y = lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_geti(L, -1, 3);
        QString type = lua_tostring(L, -1);
        lua_pop(L, 1);
        if (type == "INPUT") {
          pin_t pin;
          pin.point = QPoint(x, y);
          pin.direction = 0;
          pin.state = false;
          pin.type = false;
          pins.append(pin);
        }
        if (type == "OUTPUT") {
          pin_t pin;
          pin.point = QPoint(x, y);
          pin.direction = 2;
          pin.type = false;
          pins.append(pin);
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
      lua_getfield(L, -1, "width");
      width = lua_tointeger(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "height");
      height = lua_tointeger(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "name");
      name = QByteArray(lua_tostring(L, -1));
      lua_pop(L, 1);
      lua_pop(L, 1);
    } else
      qDebug() << "ERR1" << lua_tostring(L, -1);
  } else
    qDebug() << "ERR2" << lua_tostring(L, -1);
}

QPixmap Block::drawBlock() {
  QPixmap ret((width * Settings::final()->gridSize()) +
                  Settings::final()->gridSize() + 1,
              (height * Settings::final()->gridSize()) + 1 +
                  Settings::final()->gridSize());
  ret.fill(Qt::transparent);
  QPainter qpainter(&ret);
  qpainter.setPen(Qt::black);
  pushGlobal(L);
  Painter painter(&qpainter);
  lua_pushinteger(L, QTime::currentTime().msecsSinceStartOfDay());
  lua_setglobal(L, "time");
  lua_getglobal(L, "paintEvent");
  qpainter.setRenderHint(QPainter::Antialiasing, true);
  painter.pushMe(L);
  if (lua_pcall(L, 1, 0, 0) == LUA_OK) {
    qpainter.setRenderHint(QPainter::Antialiasing, false);
  } else
    qDebug() << "ERR3" << lua_tostring(L, -1);
  qpainter.setPen(Qt::black);
  for (int i = 0; i < pins.length(); i++) {
    QPen pen(Qt::black);
    if (pins[i].direction == 2) {
      if (getState(i))
        pen.setColor(Qt::red);
    } else {
      if (pins[i].state)
        pen.setColor(Qt::red);
    }
    pen.setWidth(3);
    qpainter.setPen(pen);
    QPoint dir(Settings::final()->gridSize() / 2.0, 0);
    if (pins[i].direction == 2)
      dir = QPoint(-Settings::final()->gridSize() / 2.0, 0);
    if (pins[i].type == true) {
      double rad = abs(dir.x()) / 4.0;
      qpainter.drawEllipse(
          (QPointF(pins[i].point * Settings::final()->gridSize())) +
              ((dir / 4.0) * 3.0),
          rad, rad);
      dir /= 2.0;
    }
    qpainter.drawLine(pins[i].point * Settings::final()->gridSize(),
                      (pins[i].point * Settings::final()->gridSize()) + dir);
    // qDebug()<<(pins[i].point*10)<<pins[i].direction<<width;
  }
  return ret;
}

Block *Block::clone() {
  Block *nblk = new Block;
  nblk->load(Block::fileName);
  return nblk;
}

void Block::onclick(QPointF where) {
  lua_getglobal(L, "onclick");
  if (!lua_isnil(L, -1)) {
    lua_pushnumber(L, where.x());
    lua_pushnumber(L, where.y());
    if (lua_pcall(L, 2, 0, 0) == LUA_OK) {

    } else
      qDebug() << "ERR4" << lua_tostring(L, -1);
  }
}

void Block::onpress(QPointF where) {
  lua_getglobal(L, "onpress");
  if (!lua_isnil(L, -1)) {
    lua_pushnumber(L, where.x());
    lua_pushnumber(L, where.y());
    if (lua_pcall(L, 2, 0, 0) == LUA_OK) {

    } else
      qDebug() << "ERR4" << lua_tostring(L, -1);
  }
}

void Block::onrelease(QPointF where) {
  lua_getglobal(L, "onrelease");
  if (!lua_isnil(L, -1)) {
    lua_pushnumber(L, where.x());
    lua_pushnumber(L, where.y());
    if (lua_pcall(L, 2, 0, 0) == LUA_OK) {

    } else
      qDebug() << "ERR4" << lua_tostring(L, -1);
  }
}

bool Block::getState(int pin) {
  pushGlobal(L);
  lua_getglobal(L, "getState");
  if (!lua_isnil(L, -1)) {
    lua_pushinteger(L, pin + 1);
    if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
      return lua_toboolean(L, -1);
    } else
      qDebug() << "ERR4" << lua_tostring(L, -1);
  } else
    qDebug() << "Warning: "
             << "Missing State at" << name;
  return false;
}

void Block::pushGlobal(lua_State *L) {
  lua_newtable(L);
  for (int i = 0; i < pins.length(); i++) {
    bool cur = pins[i].state;
    if (pins[i].type)
      cur = !cur;
    lua_pushboolean(L, cur);
    lua_seti(L, -2, i + 1);
  }
  lua_setglobal(L, "pins");
}

QPointF Block::pointAt(QPolygonF spline, double pos, bool cyclic) {
  if (!cyclic) {
    while (spline.length() > 1) {
      QPolygonF nspline;
      for (int i = 1; i < spline.length(); i++) {
        nspline.append(QLineF(spline[i - 1], spline[i]).pointAt(pos));
      }
      spline = nspline;
    }
  } else {
    while (spline.length() > 1) {
      QPolygonF nspline;
      for (int i = 0; i < spline.length(); i++) {
        nspline.append(
            QLineF(spline[i], spline[(i + 1) % spline.length()]).pointAt(pos));
      }
      spline = nspline;
    }
  }
  return spline.first();
}
