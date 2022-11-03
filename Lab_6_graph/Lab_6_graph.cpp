#define _USE_MATH_DEFINES
#include <GL/freeglut.h>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

void display();
void reshape(int, int);
void timer(int);
void processNormalKeys(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

float rx = 0;
float ry = 0;
float rz = -5;
float angle_x = 0;
float angle_y = 0;
float angle_z = 0;
float sc = 1;
int N, M;
double B;
GLfloat light2_position[4];
GLfloat l_direction[3] = { -light2_position[0], -light2_position[1], -light2_position[2] };


struct vec3 {
    double x;
    double y;
    double z;
};

vector<vector<vec3>> points;
vector<vector<vec3>> conus;

void filler()
{
    double ang = M_PI / (4 * N + 4);
    double ang_xz = 2 * M_PI / M;
    double alpha = 0, betha = 0;
    double Sa, Ca, Sb, Cb;
    vector<vec3> pr { 0 };
    for (int i = 0; i < N; i++)
    {
        pr.clear();
        Sa = sin(alpha);
        Ca = cos(alpha);

        pr.push_back({ Sa, Ca, 0 });

        for (int j = 1; j < M; j++)
        {
            Sb = sin(betha);
            Cb = cos(betha);

            pr.push_back({ Sa * Cb, Ca, Sa * Sb });
            betha += ang_xz;
        }

        pr.push_back({ Sa, Ca, 0 });

        points.push_back(pr);

        betha = ang_xz;
        alpha += ang;
    }

    conus.push_back(points[points.size() - 1]);
    double l = sqrt(pow(points[points.size() - 1][0].x, 2) + pow(points[points.size() - 1][0].y, 2));
    double devide = l / N;
    double X = points[points.size() - 1][0].x;
    double Y = points[points.size() - 1][0].y;
    double Sin = points[points.size() - 1][0].y / l;
    double Cos = points[points.size() - 1][0].x / l;
    double dev_cX = devide * Cos;
    double dev_cY = devide * Sin;
    betha = ang_xz;
    pr.clear();

    for (int i = 0; i < N; i++)
    {
        pr.clear();
        if (Y - dev_cY < 0)
        {
            pr.push_back({ X - dev_cX, 0, 0 });
        }
        else
            pr.push_back({ X - dev_cX, Y - dev_cY, 0 });

        for (int j = 1; j < M; j++)
        {
            Sb = sin(betha);
            Cb = cos(betha);

            if (Y - dev_cY < 0)
            {
                pr.push_back({ (X - dev_cX) * Cb, 0, (X - dev_cX) * Sb });
            }
            else
                pr.push_back({ (X - dev_cX) * Cb, Y - dev_cY, (X - dev_cX) * Sb });

            betha += ang_xz;
        }

        pr.push_back({ X - dev_cX, Y - dev_cY, 0 });

        conus.push_back(pr);
        X -= dev_cX;
        Y -= dev_cY;
        betha = ang_xz;
    }
}

void init()
{
    glClearColor(0, 0, 0, 1.0); // очищаем экран в цвет, установленый параметрами R G B A
    glEnable(GL_DEPTH_TEST);

}

void init_l() {
    /*glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHT0);
    glLightfv(, GL_SPECULAR, const GLfloat * param);
    glLightfv(, GL_POSITION, const GLfloat * param); 
    glLightfv(, GL_SPOT_DIRECTION, const GLfloat * param);*/

    glClearColor(0.3, 0.3, 0.3, 0.0);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
}

int main(int argc, char** argv)
{
    float Lx, Ly, Lz;
    string response;
    cout << "inp or auto: ";
    cin >> response;

    N = 20; M = 90;
    filler();
    
    if (response == "auto")
    {
        light2_position[0] = 1;
        light2_position[1] = points[N - 1][0].y;
        light2_position[2] = 1;
        light2_position[3] = 1.0;
    }
    else {
        cout << "Input Lx: ";
        cin >> Lx;
        cout << "Input Ly: ";
        cin >> Ly;
        cout << "Input Lz: ";
        cin >> Lz;

        light2_position[0] = Lx;
        light2_position[1] = Ly;
        light2_position[2] = Lz;
        light2_position[3] = 1.0;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowPosition(200, 200);
    glutInitWindowSize(900, 900);

    glutCreateWindow("win-1");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(processNormalKeys);
    glutMouseFunc(mouse);

    init();
    init_l();

    glutMainLoop();
    return 0;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat material_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);

    GLfloat light2_diffuse[] = { 1, 1, 1 };
    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.5);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.4);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.2);

    glLoadIdentity();

    glTranslatef(rx, ry, rz);
    glRotatef(angle_x, 1, 0, 0);
    glRotatef(angle_y, 0, 1, 0);
    glRotatef(angle_z, 0, 0, 1);
    glScalef(sc, sc, sc);
    //glPointSize(6.0);
    //draw
    

    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(5, 0, 0);//X

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 5, 0);//Y

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 5);//Z

    glEnd();

    glTranslatef(0, points[N - 1][0].y, 0);
    glRotatef(90, 1, 0, 0);
    glutSolidCone(points[points.size() - 1][0].x, points[points.size() - 1][0].y, 90, 20);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, -points[N - 1][0].y, 0);

    vector<vec3> p;

    //hat
    vector<vec3> tr;
    vec3 vNormal;
    vec3 normed_light;
    double coeff;

    vector<vec3> prev;
    for (int i = 1; i < points.size(); i++)
    {
        p.clear();
        prev.clear();
        p = points[i];
        prev = points[i - 1];

        for (int j = 1; j < p.size(); j++)
        {
            tr.clear();

            glBegin(GL_POLYGON);

            glNormal3f(prev[j - 1].x, prev[j - 1].y, prev[j - 1].z);
            glVertex3f(prev[j - 1].x, prev[j - 1].y, prev[j - 1].z);

            glNormal3f(prev[j].x, prev[j].y, prev[j].z);
            glVertex3f(prev[j].x, prev[j].y, prev[j].z);

            glNormal3f(p[j].x, p[j].y, p[j].z);
            glVertex3f(p[j].x, p[j].y, p[j].z);

            glNormal3f(p[j - 1].x, p[j - 1].y, p[j - 1].z);
            glVertex3f(p[j - 1].x, p[j - 1].y, p[j - 1].z);

            glEnd();
        }
    }   //рисование шара квадратами

    //glTranslatef(0, -points[N - 1][0].y, 0);

    //for (int i = 1; i < conus.size(); i++)
    //{
    //    p.clear();
    //    prev.clear();
    //    p = conus[i];
    //    prev = conus[i - 1];

    //    for (int j = 1; j < p.size(); j++)
    //    {
    //        tr.clear();

    //        glBegin(GL_POLYGON);

    //        glNormal3f(prev[j - 1].x, prev[j - 1].y, prev[j - 1].z);
    //        glVertex3f(prev[j - 1].x, prev[j - 1].y, prev[j - 1].z);

    //        glNormal3f(prev[j].x, prev[j].y, prev[j].z);
    //        glVertex3f(prev[j].x, prev[j].y, prev[j].z);

    //        glNormal3f(p[j].x, p[j].y, p[j].z);
    //        glVertex3f(p[j].x, p[j].y, p[j].z);

    //        glNormal3f(p[j - 1].x, p[j - 1].y, p[j - 1].z);
    //        glVertex3f(p[j - 1].x, p[j - 1].y, p[j - 1].z);

    //        glEnd();
    //    }
    //}   //рисование конуса с вершиной в центре координат


    glutSwapBuffers();
}


void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 2, 50);
    glMatrixMode(GL_MODELVIEW);

}
void timer(int k)
{
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}
void mouse(int button, int state, int x, int y)
{
    if (button == 3)
    {
        rz += 0.1;
    }
    else if (button == 4)
    {
        rz -= 0.1;
    }

    /*else if (button == GLUT_RIGHT_BUTTON)
    {
        leftMouseButtonDown = (state == GLUT_DOWN);
        zMovement -= 0.1f;
    }*/

    // Save the mouse position
    ///mouseXPos = x;
    ///mouseYPos = y;
    glutPostRedisplay();
}
void processNormalKeys(unsigned char key, int x, int y) {
    switch (key)
    {
    case 's': angle_x -= 2; break;
    case 'w': angle_x += 2; break;
    case 'e': angle_z += 2; break;
    case 'q': angle_z -= 2; break;
    case 'd': angle_y -= 2; break;
    case 'a': angle_y += 2; break;
    default: exit(0);
    }
    glutPostRedisplay();
}