/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <ctype.h>
#include "glv_draw.h"
#include "glv_font.h"

namespace glv{
namespace draw{

int printError(const char * pre, bool verbose, FILE * out){
	GLenum err = glGetError();
	#define CS(v, desc) case GL_##v: printf("%s%s%s\n", pre, #v, verbose?": "desc :""); break;	
	switch(err){
		CS(INVALID_ENUM, "An unacceptable value is specified for an enumerated argument.")
		CS(INVALID_VALUE, "A numeric argument is out of range.")
		CS(INVALID_OPERATION, "The specified operation is not allowed in the current state.")
		CS(STACK_OVERFLOW, "This command would cause a stack overflow.")
		CS(STACK_UNDERFLOW, "This command would cause a stack underflow.")
		CS(OUT_OF_MEMORY, "There is not enough memory left to execute the command.")
		CS(TABLE_TOO_LARGE, "The specified table exceeds the implementation's maximum supported table size.")
		default:;
	}
	#undef CS
	return int(err);
}


void fog(float end, float start, const Color& c){
	glFogi(GL_FOG_MODE, GL_LINEAR); 
	glFogf(GL_FOG_START, start); glFogf(GL_FOG_END, end);
	float fogColor[4] = {c.r, c.g, c.b, c.a};
	glFogfv(GL_FOG_COLOR, fogColor);
}


void grid(float l, float t, float w, float h, float divx, float divy, bool incEnds){
	double inc, r=l+w, b=t+h;

	if(divy > 0){
		inc = (double)h/(double)divy;
		double i = incEnds ? t-0.0001 : t-0.0001+inc;
		double e = incEnds ? b : b-inc;

		Point2 xy[2*int((e-i)/inc)];
		int ind=-1;
		for(; i<e; i+=inc){ xy[++ind](l,i); xy[++ind](r,i); }
		
		paint(Lines, xy, ind+1);
	}

	if(divx > 0){
		inc = (double)w/(double)divx;
		double i = incEnds ? l-0.0001 : l-0.0001+inc;
		double e = incEnds ? r : r-inc;

		Point2 xy[2*int((e-i)/inc)];
		int ind=-1;
		for(; i<e; i+=inc){ xy[++ind](i,t); xy[++ind](i,b); }
		
		paint(Lines, xy, ind+1);
	}
}


void linesH(float l, float t, float w, float h, int n){
	double pos = 0;
	double inc = (double)h/(double)(n-1);
		
	push();
	translate(l,t);

	Point2 pts[n*2];

	for(int i=0; i<n*2; i+=2){
		pts[i+0](0, pos);
		pts[i+1](w, pos);
		pos+=inc;
	}

	paint(Lines, pts, GLV_ARRAY_SIZE(pts));
	pop();
}


void linePattern(float l, float t, float w, float h, int n, const char * pat){
	const char * p = pat;
	double pos = 0;
	double inc = (double)w/(double)(n-1);
	int nDraw=0;
	Point2 pts[n*2];
	
	push();
	translate(l,t);

	for(int i=0; i<n; ++i){
		if(!*p) p = pat;
		if(*p++ != ' '){
			pts[nDraw+0](pos, 0);
			pts[nDraw+1](pos, h);
			nDraw += 2;
		}
		pos+=inc;
	}
	
	paint(Lines, pts, nDraw);
	pop();
}


void pgon(float l, float t, float w, float h, int n, float a){
	w *= 0.5; h *= 0.5;
	push(); translate(l + w, t + h); scale(w, h); rotate(0, 0, a * 360);

	Point2 pts[n];
	for(int i=0; i<n; ++i){
		float p = (double)i / (double)n * C_2PI;
		pts[i](cos(p), sin(p));
	}
	paint(LineLoop, pts, GLV_ARRAY_SIZE(pts));

	pop();
}


// we need to push and pop matrices and viewport bit
void push2D(float w, float h){
							// to ensure polygon edges blend properly
	disable << DepthTest << PolygonSmooth;
	enable << Blend << LineSmooth;
	blendTrans();

	push(Projection); pushAttrib(ViewPortBit); identity();
		viewport(0, 0, w, h);
		ortho(0, w, h, 0);		// flat 2D world dimension L,R,B,T
	
	push(ModelView); identity();
}


void pop2D(){
	popAttrib();					// for popping GL_VIEWPORT_BIT
	pop(Projection);
	pop(ModelView);
}


void push3D(float w, float h, float near, float far, float fovy){
	pushAttrib(ColorBufferBit | DepthBufferBit | EnableBit | ViewPortBit);
	enable(DepthTest);
	
	push(Projection); identity();
		gluPerspective(fovy, w/(GLfloat)h, near, far);
	
	push(ModelView); identity();
		translate(0, 0, -2.42);
}


void pop3D(){
	popAttrib();					// for popping GL_DEPTH_BUFFER_BIT
	pop(Projection);
	pop(ModelView);
}


void spokes(float l, float t, float w, float h, int n, float a){
	w *= 0.5; h *= 0.5;
	push();
	translate(l + w, t + h); scale(w, h); rotate(0, 0, a * 360);

	Point2 pts[2*n];
	for(int i=0; i<n; ++i){
		float p = (double)i / (double)n * C_2PI;
		pts[2*i+0](0,0);
		pts[2*i+1](cos(p), sin(p));
	}
	
	paint(Lines, pts, 2*n);
	pop();
}


void text(const char * s, float l, float t, unsigned fontSize, float lineSpacing, unsigned tabSpaces){

	Font f;
	f.size(fontSize);
	f.lineSpacing(lineSpacing);
	f.tabSpaces(tabSpaces);
	f.render(s, l,t,0);
	//f.render(s, int(l)+0.5, int(t)+0.5,0);

//	float dx = Glyph::baseline();
//	float x=l, y=t, tabUnits = tabSpaces * dx;
//	//x = (int)x + 0.5; y = (int)y + 0.5;
//	//begin(Lines);
//	while(*s){
//		switch(*s){
//			case '\t':	x = ((int)(x/tabUnits) + 1) * tabUnits; break;
//			case '\r':
//			case '\n':	x = l; y += dx * 2.f * lineSpacing; break;
//			case '\b':	x -= dx; break;
//			default:	if(character(*s, x, y)) x += dx;
//		}
//		s++;
//	}
//	//end();
}

} // draw::
} // glv::

