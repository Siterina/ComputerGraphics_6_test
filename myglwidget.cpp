#include "myglwidget.h"

#include <QtWidgets>
#include <QtOpenGL>
#include <QDebug>

#define pi 3.1415

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    radius = 1, count = 20;
    xRot = 0, yRot = 0, zRot = 0;
    xMove = 0, yMove = 0, zMove = 0;
    xScale = 1, yScale = 1, zScale = 1;
    xLightPos = 2, yLightPos = 0, zLightPos = 10;
    xMaterialColor = 1, yMaterialColor = 0.1, zMaterialColor = 1;
    xLightColor = 1, yLightColor = 1, zLightColor = 1;
    materialShiness = 10;

}

QVector3D MyGLWidget::figurePoint(double phi, double psi) {
    QVector3D temp;
    temp.setX((double)(radius * sin(phi) * cos(psi)));
    temp.setY((double)(radius * sin(phi) * sin(psi)));
    temp.setZ((double)(radius * cos(phi)));
    return temp;
}

QVector3D MyGLWidget::bottomPoint(double phi) {
    QVector3D temp;
    temp.setX(radius * cos(phi));
    temp.setY(radius * sin(phi));
    temp.setZ(0);
    return temp;
}

void MyGLWidget::addToMassive(QVector <QVector3D> tempPoint, QVector3D n){
    unsigned long long size = tempPoint.size();
    for(unsigned long long i = 0; i < size; i++) {
        vertex.push_back(tempPoint[i].x());
        vertex.push_back(tempPoint[i].y());
        vertex.push_back(tempPoint[i].z());

        normal.push_back(n.x());
        normal.push_back(n.y());
        normal.push_back(n.z());

        /*if(i == 0) {
            textures.push_back(0.0);
            textures.push_back(0.0);
        }
        if(i == 1) {
            textures.push_back(1.0);
            textures.push_back(0.0);
        }
        if(i == 2) {
            textures.push_back(1.0);
            textures.push_back(1.0);
        }
        if(i == 3) {
            textures.push_back(0.0);
            textures.push_back(1.0);
        }*/

        index.push_back(index.size());
    }
}

void MyGLWidget::countFigurePoints() {

    vertex.clear();
    normal.clear();
    index.clear();
    double step_phi = pi / count;// / 2;
    double step_psi = pi / count * 2;
    double phi = pi/360;
    double psi = 0;
    QVector <QVector3D> tempPoint;
    QVector3D n;

    for(int j = 0; j < count; psi += step_psi, j++) {
        phi = 0;
        for(int i = 0; i < count; phi += step_phi, i++) {

         tempPoint.push_back(figurePoint(phi, psi));
         tempPoint.push_back(figurePoint(phi + step_phi, psi));
         tempPoint.push_back(figurePoint(phi + step_phi, psi + step_psi));
         tempPoint.push_back(figurePoint(phi, psi + step_psi));

         textures.push_back(psi/(2 * pi));
         textures.push_back((pi - phi)/pi);
         //qDebug() << psi/(2 * pi) << endl;
         //qDebug() << psi/(2 * pi) << endl;
         textures.push_back(psi/(2 * pi));
         textures.push_back((pi - phi - step_phi)/pi);

         textures.push_back((psi + step_psi)/(2 * pi));
         textures.push_back((pi - phi - step_phi)/pi);

         textures.push_back((psi + step_psi)/(2 * pi));
         textures.push_back((pi - phi)/pi);


         n = QVector3D::normal(tempPoint[1] - tempPoint[0], tempPoint[3] - tempPoint[0]);

         addToMassive(tempPoint, n);
         tempPoint.clear();
        }
    }
    phi = pi/360;
    QVector3D temp;
    temp.setX(0);
    temp.setY(0);
    temp.setZ(0);

    for(int i = 0; i < count + 1; phi += step_psi, i++) {
        tempPoint.push_back(bottomPoint(phi));
        tempPoint.push_back(temp);
        tempPoint.push_back(temp);
        tempPoint.push_back(bottomPoint(phi + step_psi));
        n = QVector3D::normal(tempPoint[1] - tempPoint[0], tempPoint[3] - tempPoint[0]);
        addToMassive(tempPoint, n);
        tempPoint.clear();
    }

}

GLuint textureID[1];
void MyGLWidget::genTextures() {
    textureID[0] = bindTexture(QPixmap(QString("../textures/to_test_2_1024x512.png")), GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // при фильтрации игнорируются тексели, выходящие за границу текстуры для s координаты
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // при фильтрации игнорируются тексели, выходящие за границу текстуры для t координаты
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // цвет текселя полностью замещает цвет фрагмента фигуры
}

void MyGLWidget::initializeGL() {

    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    countFigurePoints();

    genTextures(); // создать текстуры
    glEnable(GL_TEXTURE_2D); // установить режим двумерных текстур

    qDebug() << "trueInitialize" << endl;
}

void MyGLWidget::paintGL() {

    countFigurePoints();
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2,2,-2,2,-2,2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLfloat lightPosition[4] = {xLightPos, yLightPos, zLightPos, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    GLfloat lightColor[4]={xLightColor,yLightColor,zLightColor,1};
    glLightfv(GL_LIGHT0,GL_DIFFUSE, lightColor);

    GLfloat materialColor[]={xMaterialColor,yMaterialColor,zMaterialColor,1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialColor);

    //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShiness);

    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);

    glTranslatef(xMove, yMove, zMove);
    glRotatef(-xRot,1,0,0);
    glRotatef(yRot,0,1,0);
    glRotatef(-zRot,0,0,1);
    glScalef(xScale,yScale,zScale);


    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,vertex.data());
    //glNormalPointer(GL_FLOAT,0,normal.data());

    glTexCoordPointer(2, GL_FLOAT, 0, textures.data()); // указываем, откуда нужно извлечь данные о массиве текстурных координат
   // glDrawElements(GL_TRIANGLES, vecIndices.size(), GL_UNSIGNED_INT, vecIndices.begin()); // строим поверхности
    glDrawElements(GL_QUADS,index.size(),GL_UNSIGNED_INT,index.data());
    //glDrawElements(GL_QUADS,index.size(),GL_UNSIGNED_INT,index.data());

}

void MyGLWidget::resizeGL(int width, int height){
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

/*******************************************/
/*                Slots                    */
/*******************************************/


void MyGLWidget::setXRotation(int angle) {
    xRot = (double)angle;
    updateGL();
}

void MyGLWidget::setYRotation(int angle) {
    yRot = (double)angle;
    updateGL();
}

void MyGLWidget::setZRotation(int angle) {
    zRot = (double)angle;
    updateGL();
}

void MyGLWidget::moveXDirection(int shift) {
    xMove = (float)shift/10;
    updateGL();
}

void MyGLWidget::moveYDirection(int shift) {
    yMove = (float)shift/10;
    updateGL();
}

void MyGLWidget::moveZDirection(int shift) {
    zMove = (float)shift/10;
    updateGL();
}

void MyGLWidget::scaleXDirection(int scale) {
    xScale = (float)scale;
    updateGL();
}

void MyGLWidget::scaleYDirection(int scale) {
    yScale = (float)scale;
    updateGL();
}

void MyGLWidget::scaleZDirection(int scale) {
    zScale = (float)scale;
    updateGL();
}

void MyGLWidget::setXLightPos(int position) {
    xLightPos = (float)position;
    updateGL();
}

void MyGLWidget::setYLightPos(int position) {
    yLightPos = (float)position;
    updateGL();
}

void MyGLWidget::setZLightPos(int position) {
    zLightPos = (float)position;
    updateGL();
}

void MyGLWidget::setXMaterialColor(int color) {
    xMaterialColor = (float)color/255;
    updateGL();
}

void MyGLWidget::setYMaterialColor(int color) {
    yMaterialColor = (float)color/255;
    updateGL();
}

void MyGLWidget::setZMaterialColor(int color) {
    zMaterialColor = (float)color/255;
    updateGL();
}

void MyGLWidget::setXLightColor(int color) {
    xLightColor = (float)color/255;
    updateGL();
}

void MyGLWidget::setYLightColor(int color) {
    yLightColor = (float)color/255;
    updateGL();
}

void MyGLWidget::setZLightColor(int color) {
    zLightColor = (float)color/255;
    updateGL();
}

/*void MyGLWidget::setMaterialShiness(int intence) {
} */

void MyGLWidget::setApproximation(int aCount) {
    count = aCount;
    updateGL();
}
