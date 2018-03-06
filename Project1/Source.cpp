#include <stdio.h> 
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <time.h>
#include "helpers.h"
#include "bone.h"
#include "movement.h"
#include "ccd.h"


float speed_x = 0; 
float speed_y = 0; 
int lastTime = 0;
float angle_x = -30;
float angle_y = 0;
float zoom = 15.0f;
float cdx, cdy;

glm::vec3 target;
Movement* animation = new Movement();
bool animation_running = false;
float animation_fill = 0.0f;
bool root_set = false;
unsigned long long effector = 0;

GLfloat mat_podstawa[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat mat_ramie[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat mat_staw[] = {1.0, 0.0, 0.0, 1.0 };
GLfloat mat_dlon[] = { 1.0,1.0,0.0, 1.0 };
GLfloat mat_cel[] = { 1.0,1.0,1.0, 1.0 };

Bone* root;
float r_up = 60.0f;

void randomizeTarget() {
	target.x = float(rand() % 10000) / 2000.0;
	target.y = float(rand() % 10000) / 2000.0;
	target.z = float(rand() % 10000) / 2000.0;

	GLfloat light_position[] = { target.x, target.y, target.z, 1.0 };


	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	mat_cel[3] = 0.5;
	glLightfv(GL_LIGHT1, GL_DIFFUSE, mat_cel);
	mat_cel[3] = 1.0;

	delete animation;
	animation = new Movement();

}

void drawBones(Bone* b) {
	glPushMatrix();
	GLfloat s[] = { 0 };

	glm::mat4 previous = b->M;

	if (b->parent != NULL) {
		b->M = glm::translate(b->M, glm::vec3(0.0f, 0.0f, b->parent->length));

		b->M = glm::rotate(b->M, b->rotation.x, glm::vec3(b->M*glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
		b->M = glm::rotate(b->M, b->rotation.y, glm::vec3(b->M*glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
		b->M = glm::rotate(b->M, b->rotation.z, glm::vec3(b->M*glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));

		b->M = b->parent->M * b->M;
	}
	else {
		b->M = glm::rotate(b->M, b->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		b->M = glm::rotate(b->M, b->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		b->M = glm::rotate(b->M, b->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glLoadMatrixf(glm::value_ptr(b->M));

	if (b->parent != NULL) {
		s[0] = 250;
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_staw);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_staw);
		glMaterialfv(GL_FRONT, GL_SHININESS, s);
		glutSolidSphere(0.2f, 32, 32);
	}

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_ramie);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_ramie);
	s[0] = 11;
	glMaterialfv(GL_FRONT, GL_SHININESS, s);

	GLUquadricObj* q = gluNewQuadric();
	if (b->bones.empty())
		gluCylinder(q, 0.1f, 0.1f, b->length - 0.5f, 32, 32);
	else
		gluCylinder(q, 0.1f, 0.1f, b->length, 32, 32);
	gluDeleteQuadric(q);

	glPopMatrix();


	if (b->bones.empty()) {
		glPushMatrix();

		glLoadMatrixf(glm::value_ptr(glm::translate(b->M, glm::vec3(0.0f, 0.0f, b->length - 0.5f))));
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_dlon);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dlon);
		s[0] = 250;
		glMaterialfv(GL_FRONT, GL_SHININESS, s);

		gluCylinder(q, 0.2f, 0.0f, 0.5f, 32, 32);

		glPopMatrix();
	}

	for (std::vector<Bone*>::iterator it = b->bones.begin(); it != b->bones.end(); it++) {
		drawBones(*it);
	}

	b->M = previous;
}

void displayFrame(void) {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 cameraCenter = glm::vec3(0.0f + cdx, 2.0f + cdy, 0.0);
	glm::vec3 cameraEye = glm::vec3(0.0f, 1.0f, -zoom);
	glm::vec3 cameraNose = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 V = glm::lookAt(cameraEye, cameraCenter, cameraNose);
	V = glm::rotate(V, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
	V = glm::rotate(V, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 P = glm::perspective(50.0f, 1.0f, 1.0f, 50.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(P));
	glMatrixMode(GL_MODELVIEW);


	glm::mat4 M = glm::mat4(1.0f);
	glLoadMatrixf(glm::value_ptr(V*M));

	GLfloat s[] = { 128 };
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_cel);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_cel);
	glMaterialfv(GL_FRONT, GL_SHININESS, s);
	mat_cel[3] = 0.5;
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_cel);
	mat_cel[3] = 1.0;
	glLoadMatrixf(glm::value_ptr(glm::translate(M*V, target)));
	glutSolidSphere(0.5f, 32, 32);
	glPopMatrix();

	GLfloat t[] = { 0.0,0.0, 0.0,1.0 };
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_podstawa);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_podstawa);
	s[0] = 0;
	glMaterialfv(GL_FRONT, GL_SHININESS, s);
	glMaterialfv(GL_FRONT, GL_EMISSION, t);
	glScalef(0.6f, 0.2f, 0.6f);
	glutSolidCube(1.0f);
	glPopMatrix();

	root->M = glm::rotate(M*V, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	root->setRotate(root->rotation.x, root->rotation.y, root->rotation.z);


	drawBones(root);


	glutSwapBuffers();
}


void nextFrame(void) {
	int actTime = glutGet(GLUT_ELAPSED_TIME);
	float interval = actTime - lastTime;
	lastTime = actTime;
	angle_x += speed_x*interval / 1000.0;
	angle_y += speed_y*interval / 1000.0;

	if (animation_running) {
		animation_fill += interval / 2000.0;

		if (!animation->frame(interval / 2000.0, root)) {
			animation_fill = 0.0;
			if (!animation->next()) {
				animation_running = false;
			}
		}
	}

	if (angle_x>360) angle_x -= 360;
	if (angle_x>360) angle_x += 360;
	if (angle_y>360) angle_y -= 360;
	if (angle_y>360) angle_y += 360;

	glutPostRedisplay();
}

void specKeyDown(int c, int x, int y) {
	if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
		switch (c) {
		case GLUT_KEY_LEFT:
			cdx -= 0.25;
			break;
		case GLUT_KEY_RIGHT:
			cdx += 0.25;
			break;
		case GLUT_KEY_UP:
			cdy -= 0.25;
			break;
		case GLUT_KEY_DOWN:
			cdy += 0.25;
			break;
		}
	}
	else {
		switch (c) {
		case GLUT_KEY_LEFT:
			speed_y = 60;
			break;
		case GLUT_KEY_RIGHT:
			speed_y = -60;
			break;
		case GLUT_KEY_UP:
			speed_x = 60;
			break;
		case GLUT_KEY_DOWN:
			speed_x = -60;
			break;
		}
	}
}

void specKeyUp(int c, int x, int y) {
	switch (c) {
	case GLUT_KEY_LEFT:
		speed_y = 0;
		break;
	case GLUT_KEY_RIGHT:
		speed_y = -0;
		break;
	case GLUT_KEY_UP:
		speed_x = 0;
		break;
	case GLUT_KEY_DOWN:
		speed_x = -0;
		break;
	}
}

void keyDown(unsigned char c, int x, int y) {
	try {
		switch (c) {
		case '-':
			zoom += 0.5f;
			break;
		case '=':
		case '+':
			zoom -= 0.5f;
			break;

		case 'u':
			root->bone(111)->rotate(-1.0f, 0.0f, 0.0f);
			break;
		case 'j':
			root->bone(111)->rotate(+1.0f, 0.0f, 0.0f);
			break;
		case 'i':
			root->bone(111)->rotate(0.0f, -1.0f, 0.0f);
			break;
		case 'k':
			root->bone(111)->rotate(0.0f, +1.0f, 0.0f);
			break;
		case 'o':
			root->bone(111)->rotate(0.0f, 0.0f, -1.0f);
			break;
		case 'l':
			root->bone(111)->rotate(0.0f, 0.0f, +1.0f);
			break;


		case 'r':
			root->bone(11)->rotate(-1.0f, 0.0f, 0.0f);
			break;
		case 'f':
			root->bone(11)->rotate(+1.0f, 0.0f, 0.0f);
			break;
		case 't':
			root->bone(11)->rotate(0.0f, -1.0f, 0.0f);
			break;
		case 'g':
			root->bone(11)->rotate(0.0f, +1.0f, 0.0f);
			break;
		case 'y':
			root->bone(11)->rotate(0.0f, 0.0f, -1.0f);
			break;
		case 'h':
			root->bone(11)->rotate(0.0f, 0.0f, +1.0f);
			break;

		case 'q':
			root->bone(1)->rotate(-1.0f, 0.0f, 0.0f);
			break;
		case 'a':
			root->bone(1)->rotate(+1.0f, 0.0f, 0.0f);
			break;
		case 'w':
			root->bone(1)->rotate(0.0f, -1.0f, 0.0f);
			break;
		case 's':
			root->bone(1)->rotate(0.0f, +1.0f, 0.0f);
			break;
		case 'e':
			root->bone(1)->rotate(0.0f, 0.0f, -1.0f);
			break;
		case 'd':
			root->bone(1)->rotate(0.0f, 0.0f, +1.0f);
			break;
		case ' ':
			randomizeTarget();
			break;

		case 'Q':
			target.z -= 0.2f;
			break;
		case 'E':
			target.z += 0.2f;
			break;
		case 'W':
			target.y += 0.2;
			break;
		case 'S':
			target.y -= 0.2;
			break;
		case 'A':
			target.x -= 0.2;
			break;
		case 'D':
			target.x += 0.2;
			break;
		}
	}
	catch (ConstraintException* e) {
		printf("Cannot move further!\n");
	}
}

void displayVec3(glm::vec3 vec) {
	printf("x: %f, y: %f, z: %f\n", vec.x, vec.y, vec.z);
}

void displayVec3(glm::vec4 vec) {
	printf("x: %f, y: %f, z: %f\n", vec.x, vec.y, vec.z);
}

void keyUp(unsigned char c, int x, int y) {
	switch (c) {
	case '.':
		ccd::findNewAngles(root->bone(effector), target);
		break;
	case ',':
		Bone *b = new Bone(*root);

		ccd::findNewAngles(b->bone(effector), target);

		delete animation;
		animation = new Movement();

		animation->set(root)->keyframe();

		for (int z = effector; z != 0; z /= 10) {
			animation->set(b->bone(z), b->bone(z)->rotation)->keyframe();
		}

		delete b;

		animation->start()->next();
		animation_running = true;

		break;
	}
}


int main(int argc, char* argv[]) {
	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Inverse Kinematics - Adwitiya Chakraborty - 17320705");
	glutDisplayFunc(displayFrame);
	glutIdleFunc(nextFrame);

	glutSpecialFunc(specKeyDown);
	glutSpecialUpFunc(specKeyUp);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);

	glShadeModel(GL_SMOOTH);

	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat light_position[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_specular[] = { 0.5, 0.5, 0.5, 1.0 };


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	randomizeTarget();


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glEnable(GL_DEPTH_TEST);

	root = new Bone(0.0f);

	// root->add(new Bone(3.5))->constraints(-90,90, -90,90, -360,360)->rotate(0, 0, 0)
	//     ->add(new Bone(4))->rotate(0, -75, 0)
	//     ->add(new Bone(1))->rotate(0, -45, 0);
	//  effector = 111;
	root->add(new Bone(1))->add(new Bone(1))->rotate(0, 10, 0)
		->add(new Bone(1))->rotate(0, 10, 0)
		->add(new Bone(1))->rotate(0, 10, 0)
		->add(new Bone(1))->rotate(0, 10, 0)
		->add(new Bone(1))->rotate(0, 10, 0)->add(new Bone(1))->rotate(0, 10, 0)->add(new Bone(1))->rotate(0, 10, 0);

	effector = 11111111;

	glutMainLoop();

	delete root;
	delete animation;

	return 0;
}
