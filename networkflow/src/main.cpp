#include "flow.hpp"
#include <SDL.h>
#include <SDL_opengl.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr float pi = 3.14159265f;
constexpr float panel = 312;
struct Vec {
    float x, y;
    Vec operator+(Vec b) const { return {x+b.x,y+b.y}; }
    Vec operator-(Vec b) const { return {x-b.x,y-b.y}; }
    Vec operator*(float s) const { return {x*s,y*s}; }
};
float length(Vec a) { return std::sqrt(a.x*a.x+a.y*a.y); }
struct Color { float r,g,b; };
constexpr Color background{0.047f,0.071f,0.11f}, surface{0.075f,0.102f,0.15f};
constexpr Color ink{0.88f,0.92f,0.97f}, muted{0.49f,0.59f,0.70f};
constexpr Color teal{0.24f,0.89f,0.72f}, blue{0.36f,0.64f,1.0f}, gold{1.0f,0.73f,0.35f};
void color(Color c) { glColor3f(c.r,c.g,c.b); }
void rect(float x,float y,float w,float h,Color c) {
    color(c); glBegin(GL_QUADS);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h); glEnd();
}
void circle(Vec p,float r,Color c) {
    color(c); glBegin(GL_TRIANGLE_FAN); glVertex2f(p.x,p.y);
    for(int i=0;i<=40;++i) { float a=2*pi*i/40; glVertex2f(p.x+r*std::cos(a),p.y+r*std::sin(a)); } glEnd();
}
// Original compact 5x7 bitmap alphabet; rendered as OpenGL quads, no font assets.
std::array<unsigned char,7> glyph(char c) {
    switch(c) {
    case 'A': return {14,17,17,31,17,17,17}; case 'B': return {30,17,17,30,17,17,30};
    case 'C': return {14,17,16,16,16,17,14}; case 'D': return {30,17,17,17,17,17,30};
    case 'E': return {31,16,16,30,16,16,31}; case 'F': return {31,16,16,30,16,16,16};
    case 'G': return {14,17,16,23,17,17,15}; case 'H': return {17,17,17,31,17,17,17};
    case 'I': return {14,4,4,4,4,4,14}; case 'J': return {7,2,2,2,18,18,12};
    case 'K': return {17,18,20,24,20,18,17}; case 'L': return {16,16,16,16,16,16,31};
    case 'M': return {17,27,21,21,17,17,17}; case 'N': return {17,25,25,21,19,19,17};
    case 'O': return {14,17,17,17,17,17,14}; case 'P': return {30,17,17,30,16,16,16};
    case 'Q': return {14,17,17,17,21,18,13}; case 'R': return {30,17,17,30,20,18,17};
    case 'S': return {15,16,16,14,1,1,30}; case 'T': return {31,4,4,4,4,4,4};
    case 'U': return {17,17,17,17,17,17,14}; case 'V': return {17,17,17,17,17,10,4};
    case 'W': return {17,17,17,21,21,21,10}; case 'X': return {17,17,10,4,10,17,17};
    case 'Y': return {17,17,10,4,4,4,4}; case 'Z': return {31,1,2,4,8,16,31};
    case '0': return {14,17,19,21,25,17,14}; case '1': return {4,12,4,4,4,4,14};
    case '2': return {14,17,1,2,4,8,31}; case '3': return {30,1,1,14,1,1,30};
    case '4': return {2,6,10,18,31,2,2}; case '5': return {31,16,16,30,1,1,30};
    case '6': return {14,16,16,30,17,17,14}; case '7': return {31,1,2,4,8,8,8};
    case '8': return {14,17,17,14,17,17,14}; case '9': return {14,17,17,15,1,1,14};
    case '.': return {0,0,0,0,0,12,12}; case ':': return {0,12,12,0,12,12,0};
    case '-': return {0,0,0,31,0,0,0}; case '/': return {1,2,2,4,8,8,16};
    case '>': return {16,8,4,2,4,8,16}; case '<': return {1,2,4,8,4,2,1};
    case '=': return {0,0,31,0,31,0,0}; case '+': return {0,4,4,31,4,4,0};
    case '(': return {2,4,8,8,8,4,2}; case ')': return {8,4,2,2,2,4,8};
    case '[': return {14,8,8,8,8,8,14}; case ']': return {14,2,2,2,2,2,14};
    case '_': return {0,0,0,0,0,0,31}; default: return {};
    }
}
float text_width(const std::string& text,float scale) { return text.size()*6*scale; }
void text(float x,float y,const std::string& value,Color c=ink,float scale=1.5f) {
    color(c); glBegin(GL_QUADS);
    for(char ch:value) {
        if(ch>='a' && ch<='z') ch=static_cast<char>(ch-'a'+'A');
        auto rows=glyph(ch);
        for(int row=0;row<7;++row) for(int col=0;col<5;++col) if(rows[row] & (1<<(4-col))) {
            float px=x+col*scale, py=y+row*scale;
            glVertex2f(px,py); glVertex2f(px+scale,py); glVertex2f(px+scale,py+scale); glVertex2f(px,py+scale);
        }
        x+=6*scale;
    }
    glEnd();
}
std::string number(double value,bool decimal) {
    std::ostringstream out; out<<std::fixed<<std::setprecision(decimal?2:0)<<value; return out.str();
}
struct Box {
    float x,y,w,h;
    bool contains(Vec p) const { return p.x>=x && p.x<=x+w && p.y>=y && p.y<=y+h; }
};
struct Field { std::string label,value; Box box; };

class App {
public:
    int width=1240,height=820;
    std::array<Field,4> fields{{{"VERTICES  (2-40)","8",{24,144,264,36}},
                               {"EDGES","14",{24,260,264,36}},
                               {"DENSITY  (0-1)","0.14",{24,318,264,36}},
                               {"SEED","4234",{24,440,264,36}}}};
    bool by_density=false,decimal=false,solved=false,used_only=false,labels=true;
    bool graph_decimal=false;
    bool labels_dirty=true;
    int active=-1,drag=-1,hover=-1;
    bool replace=false;
    Vec mouse{-100,-100};
    nf::Graph graph;
    nf::Result result;
    std::vector<Vec> positions;
    std::vector<Vec> label_positions;
    std::string status="Set parameters, then generate.";
    std::string error;

    int integer(int i) const {
        const std::string& s=fields[i].value;
        if(s.empty() || s.find_first_not_of("0123456789")!=std::string::npos) throw std::invalid_argument("Use whole numbers for vertices, edges and seed.");
        std::size_t end=0; long long value=std::stoll(s,&end);
        if(value>2147483647) throw std::invalid_argument("Whole number is too large.");
        return static_cast<int>(value);
    }
    float density() const {
        std::size_t end=0; float d=std::stof(fields[2].value,&end);
        if(end!=fields[2].value.size() || !std::isfinite(d) || d<0 || d>1) throw std::invalid_argument("Density must be between 0 and 1.");
        return d;
    }
    void layout() {
        labels_dirty=true;
        positions.resize(graph.vertices);
        float cx=panel+(width-panel)/2.0f, cy=height/2.0f;
        float rx=(width-panel)/2.0f-84, ry=height/2.0f-132;
        positions[0]={cx-rx,cy}; positions.back()={cx+rx,cy};
        // Place internal vertices on upper/lower arcs, keeping terminals at the sides.
        int top=(graph.vertices-2+1)/2, bottom=graph.vertices-2-top;
        for(int i=0;i<top;++i) {
            float a=pi+(i+1)*pi/(top+1);
            positions[i+1]={cx+rx*std::cos(a),cy+ry*std::sin(a)};
        }
        for(int i=0;i<bottom;++i) {
            float a=(i+1)*pi/(bottom+1);
            positions[top+1+i]={cx+rx*std::cos(a),cy+ry*std::sin(a)};
        }
    }
    void generate(bool new_seed=false) {
        try {
            int n=integer(0);
            if(n<2 || n>40) throw std::invalid_argument("Choose 2 to 40 vertices for the visualizer.");
            int seed=integer(3);
            if(new_seed) seed=seed==2147483647?0:seed+1;
            int count=by_density?nf::edges_for_density(n,density()):integer(1);
            auto next=nf::generate(n,count,decimal,static_cast<std::uint32_t>(seed));
            graph=std::move(next); graph_decimal=decimal;
            fields[3].value=std::to_string(seed);
            fields[1].value=std::to_string(count);
            solved=false; used_only=false; hover=-1; drag=-1; result={}; error.clear();
            layout(); status="Practice first. SPACE reveals the answer.";
        } catch(const std::exception& e) { error=e.what(); }
    }
    void run() {
        if(graph.vertices==0) return;
        result=nf::solve(graph,0,graph.vertices-1,graph_decimal?nf::Algorithm::Dinic:nf::Algorithm::EdmondsKarp);
        solved=true; labels_dirty=true; error.clear(); status="Answer revealed. R resets this exercise.";
    }
    void reset() { solved=false; used_only=false; labels_dirty=true; result={}; status="Flow hidden. SPACE reveals the answer."; }
    void button(Box b,const std::string& label,bool selected=false) {
        rect(b.x,b.y,b.w,b.h,selected?Color{0.12f,0.32f,0.32f}:Color{0.12f,0.17f,0.23f});
        text(b.x+12,b.y+(b.h-10.5f)/2,label,selected?teal:ink);
    }
    void click(Vec p) {
        active=-1; SDL_StopTextInput();
        for(int i=0;i<4;++i) {
            if((i==1 && by_density) || (i==2 && !by_density)) continue;
            if(fields[i].box.contains(p)) { active=i; replace=true; SDL_StartTextInput(); return; }
        }
        if(Box{24,204,128,30}.contains(p)) by_density=false;
        else if(Box{160,204,128,30}.contains(p)) by_density=true;
        else if(Box{24,386,128,30}.contains(p)) decimal=false;
        else if(Box{160,386,128,30}.contains(p)) decimal=true;
        else if(Box{24,494,264,38}.contains(p)) generate();
        else if(Box{24,548,264,38}.contains(p)) run();
        else if(Box{24,598,128,30}.contains(p)) reset();
        else if(Box{160,598,128,30}.contains(p)) { if(solved) { used_only=!used_only; labels_dirty=true; } }
        else for(int i=0;i<graph.vertices;++i) if(length(p-positions[i])<23) { drag=i; break; }
    }
    void event(const SDL_Event& e) {
        if(e.type==SDL_WINDOWEVENT && e.window.event==SDL_WINDOWEVENT_SIZE_CHANGED) {
            width=e.window.data1; height=e.window.data2; layout();
        } else if(e.type==SDL_MOUSEMOTION) {
            mouse={static_cast<float>(e.motion.x),static_cast<float>(e.motion.y)};
            if(drag>=0) {
                positions[drag]={std::clamp(mouse.x,panel+30,width-30.0f),std::clamp(mouse.y,120.0f,height-92.0f)};
                labels_dirty=true;
            }
        } else if(e.type==SDL_MOUSEBUTTONDOWN && e.button.button==SDL_BUTTON_LEFT) {
            click({static_cast<float>(e.button.x),static_cast<float>(e.button.y)});
        } else if(e.type==SDL_MOUSEBUTTONUP && e.button.button==SDL_BUTTON_LEFT) drag=-1;
        else if(e.type==SDL_TEXTINPUT && active>=0) {
            std::string input=e.text.text;
            if(input.find_first_not_of(active==2?"0123456789.":"0123456789")==std::string::npos) {
                if(replace) fields[active].value.clear();
                replace=false;
                if(fields[active].value.size()+input.size()<=12) fields[active].value+=input;
            }
        } else if(e.type==SDL_KEYDOWN) {
            auto key=e.key.keysym.sym;
            if(active>=0) {
                if(key==SDLK_ESCAPE || key==SDLK_RETURN || key==SDLK_KP_ENTER) { active=-1; SDL_StopTextInput(); }
                else if(key==SDLK_BACKSPACE) {
                    if(replace) fields[active].value.clear();
                    else if(!fields[active].value.empty()) fields[active].value.pop_back();
                    replace=false;
                } else if(key==SDLK_TAB) {
                    do { active=(active+1)%4; } while((active==1 && by_density)||(active==2 && !by_density));
                    replace=true;
                }
                return;
            }
            if(key==SDLK_SPACE) run();
            else if(key==SDLK_g || key==SDLK_RETURN) generate();
            else if(key==SDLK_n) generate(true);
            else if(key==SDLK_r) reset();
            else if(key==SDLK_l) labels=!labels;
            else if(key==SDLK_u && solved) { used_only=!used_only; labels_dirty=true; }
            else if(key==SDLK_c) layout();
        }
    }
    Vec point(int i,float t) const {
        const auto& e=graph.edges[i];
        Vec a=positions[e.from], b=positions[e.to], d=b-a;
        float distance=std::max(1.0f,length(d)); Vec normal{-d.y/distance,d.x/distance};
        // Opposite directions bend to opposite sides and have separate labels.
        Vec control=(a+b)*0.5f+normal*std::min(42.0f,distance*0.18f);
        return a*((1-t)*(1-t))+control*(2*(1-t)*t)+b*(t*t);
    }
    void edge(int i,bool highlighted) {
        const auto& e=graph.edges[i];
        bool flowing=solved && result.flow[i]>nf::epsilon;
        float distance=length(positions[e.to]-positions[e.from]);
        if(distance<50) return;
        float start=22/distance, end=1-24/distance;
        Color c=highlighted?gold:(flowing?teal:(solved?Color{0.19f,0.25f,0.33f}:Color{0.34f,0.43f,0.55f}));
        color(c); glLineWidth(highlighted?3.0f:(flowing?2.5f:1.25f)); glBegin(GL_LINE_STRIP);
        for(int j=0;j<=32;++j) { Vec p=point(i,start+(end-start)*j/32); glVertex2f(p.x,p.y); } glEnd();
        Vec tip=point(i,end), prev=point(i,end-0.025f), direction=tip-prev;
        direction=direction*(1/std::max(0.001f,length(direction))); Vec normal{-direction.y,direction.x};
        Vec left=tip-direction*10+normal*4, right=tip-direction*10-normal*4;
        glBegin(GL_TRIANGLES); glVertex2f(tip.x,tip.y); glVertex2f(left.x,left.y); glVertex2f(right.x,right.y); glEnd();
    }
    void edge_label(int i,bool highlighted) {
        const auto& e=graph.edges[i];
        bool flowing=solved && result.flow[i]>nf::epsilon;
        std::string label=solved?number(result.flow[i],graph_decimal)+"/"+number(e.capacity,graph_decimal):number(e.capacity,graph_decimal);
        Vec p=label_positions[i]; float size=highlighted?1.5f:1.25f, w=text_width(label,size);
        rect(p.x-w/2-4,p.y-7,w+8,15,background);
        text(p.x-w/2,p.y-4.5f,label,highlighted?gold:(flowing?teal:muted),size);
    }
    void place_labels() {
        if(!labels_dirty) return;
        labels_dirty=false;
        label_positions.resize(graph.edges.size());
        std::vector<Box> occupied;
        for(int i=0;i<static_cast<int>(graph.edges.size());++i) {
            if(used_only && result.flow[i]<=nf::epsilon) continue;
            std::string label=number(graph.edges[i].capacity,graph_decimal);
            if(solved) label=number(result.flow[i],graph_decimal)+"/"+label;
            float w=text_width(label,1.25f)+12;
            float best=1e30f; Box chosen{};
            // Keep labels near edge midpoints, moving along the curve to avoid collisions.
            for(float t:{0.5f,0.4f,0.6f,0.3f,0.7f,0.2f,0.8f}) {
                Vec p=point(i,t); Box b{p.x-w/2,p.y-9,w,18};
                float penalty=std::fabs(t-0.5f);
                for(const auto& other:occupied) {
                    float overlap_x=std::max(0.0f,std::min(b.x+b.w,other.x+other.w)-std::max(b.x,other.x));
                    float overlap_y=std::max(0.0f,std::min(b.y+b.h,other.y+other.h)-std::max(b.y,other.y));
                    penalty+=overlap_x*overlap_y;
                }
                for(Vec vertex:positions) {
                    Vec closest{std::clamp(vertex.x,b.x,b.x+b.w),std::clamp(vertex.y,b.y,b.y+b.h)};
                    if(length(vertex-closest)<30) penalty+=10000;
                }
                if(penalty<best) { best=penalty; chosen=b; label_positions[i]=p; }
            }
            occupied.push_back(chosen);
        }
    }
    void draw() {
        glClearColor(background.r,background.g,background.b,1); glClear(GL_COLOR_BUFFER_BIT);
        rect(0,0,panel,static_cast<float>(height),surface);
        text(24,30,"FLOW LAB",teal,3); text(24,65,"MANUAL TRACING PRACTICE",muted,1.3f);
        text(24,108,"01 / GENERATE",ink,1.5f);
        button({24,204,128,30},"EDGE COUNT",!by_density); button({160,204,128,30},"DENSITY",by_density);
        text(24,366,"CAPACITY TYPE",muted,1.25f);
        button({24,386,128,30},"INTEGER",!decimal); button({160,386,128,30},"DECIMAL",decimal);
        for(int i=0;i<4;++i) {
            auto& f=fields[i]; bool enabled=!((i==1&&by_density)||(i==2&&!by_density));
            text(f.box.x,f.box.y-15,f.label,enabled?muted:Color{0.28f,0.34f,0.42f},1.25f);
            rect(f.box.x,f.box.y,f.box.w,f.box.h,active==i?Color{0.13f,0.27f,0.31f}:background);
            text(f.box.x+12,f.box.y+12,f.value+(active==i?"_":""),enabled?ink:muted);
        }
        button({24,494,264,38},"G  GENERATE GRAPH",true);
        button({24,548,264,38},"SPACE  REVEAL MAX FLOW",solved);
        button({24,598,128,30},"R  RESET"); button({160,598,128,30},"U  USED",used_only);
        text(24,653,"N  NEW SEED + GRAPH",muted,1.25f);
        text(24,677,"L  TOGGLE EDGE LABELS",muted,1.25f);
        text(24,701,"C  RESET LAYOUT",muted,1.25f);
        text(24,735,"CLICK FIELDS TO EDIT",muted,1.25f);
        text(24,757,"DRAG VERTICES TO UNTANGLE",muted,1.25f);
        text(panel+28,30,graph_decimal?"DINIC / DINITZ":"EDMONDS-KARP",ink,2.2f);
        text(panel+28,62,"SOURCE 0  >  SINK "+std::to_string(graph.vertices-1)+"     "+std::to_string(graph.vertices)+" VERTICES / "+std::to_string(graph.edges.size())+" EDGES",muted,1.3f);
        if(solved) text(width-270.0f,32,"MAX FLOW "+number(result.value,graph_decimal),teal,1.6f);
        hover=-1; float nearest=10;
        for(int i=0;i<static_cast<int>(graph.edges.size());++i) {
            if(used_only && result.flow[i]<=nf::epsilon) continue;
            for(int j=3;j<=29;++j) {
                float d=length(mouse-point(i,j/32.0f));
                if(d<nearest) { nearest=d; hover=i; }
            }
            edge(i,false);
        }
        place_labels();
        for(int i=0;i<static_cast<int>(graph.edges.size());++i) {
            if(used_only && result.flow[i]<=nf::epsilon) continue;
            if(labels) edge_label(i,false);
        }
        if(hover>=0) { edge(hover,true); edge_label(hover,true); }
        for(int i=0;i<graph.vertices;++i) {
            Vec p=positions[i]; Color accent=i==0?teal:(i==graph.vertices-1?gold:blue);
            circle(p,23,accent); circle(p,20,surface);
            std::string label=std::to_string(i); text(p.x-text_width(label,1.7f)/2,p.y-6,label,ink,1.7f);
            if(i==0 || i==graph.vertices-1) {
                std::string role=i==0?"SOURCE":"SINK";
                text(p.x-text_width(role,1.1f)/2,p.y+31,role,accent,1.1f);
            }
        }
        rect(panel,height-75.0f,width-panel,75,surface);
        std::string footer=solved?"TEAL = POSITIVE FLOW    LABEL = FLOW / CAPACITY":"ARROW = DIRECTION    LABEL = CAPACITY";
        text(panel+24,height-58.0f,footer,solved?teal:muted,1.25f);
        if(hover>=0) {
            const auto& e=graph.edges[hover];
            footer="EDGE "+std::to_string(e.from)+" > "+std::to_string(e.to)+"    CAPACITY "+number(e.capacity,graph_decimal);
            if(solved) footer+="    FLOW "+number(result.flow[hover],graph_decimal);
        } else footer=error.empty()?status:error;
        // Wrap validation text on narrow windows.
        std::size_t limit=static_cast<std::size_t>((width-panel-48)/7.5f);
        text(panel+24,height-34.0f,footer.substr(0,limit),error.empty()?ink:gold,1.25f);
        if(footer.size()>limit) text(panel+24,height-19.0f,footer.substr(limit,limit),gold,1.25f);
    }
};

void screenshot(const std::string& path,int width,int height) {
    std::vector<unsigned char> pixels(static_cast<std::size_t>(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1); glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    std::ofstream output(path,std::ios::binary);
    if(!output) throw std::runtime_error("Cannot write screenshot: "+path);
    output<<"P6\n"<<width<<" "<<height<<"\n255\n";
    for(int y=height-1;y>=0;--y) output.write(reinterpret_cast<const char*>(pixels.data()+static_cast<std::size_t>(y)*width*3),width*3);
}
} // namespace

int main(int argc,char** argv) {
    const bool smoke=argc==3 && std::string(argv[1])=="--smoke-test";
    if(argc>1 && !smoke) { std::cout<<"Usage: networkflow [--smoke-test screenshot-prefix]\n"; return 0; }
    SDL_SetMainReady();
    if(SDL_Init(SDL_INIT_VIDEO)!=0) { std::cerr<<SDL_GetError()<<'\n'; return 1; }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_Window* window=SDL_CreateWindow("Flow Lab | Network Flow Practice",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1240,820,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
    if(!window) { std::cerr<<SDL_GetError()<<'\n'; SDL_Quit(); return 1; }
    SDL_SetWindowMinimumSize(window,1100,800);
    SDL_GLContext context=SDL_GL_CreateContext(window);
    if(!context) { std::cerr<<SDL_GetError()<<'\n'; SDL_DestroyWindow(window); SDL_Quit(); return 1; }
    SDL_GL_SetSwapInterval(1);
    int exit_code=0;
    try {
        App app; app.generate(); bool running=true; int frame=0;
        while(running) {
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                if(event.type==SDL_QUIT) running=false;
                else app.event(event);
            }
            int w=0,h=0; SDL_GL_GetDrawableSize(window,&w,&h);
            glViewport(0,0,w,h); glMatrixMode(GL_PROJECTION); glLoadIdentity();
            glOrtho(0,app.width,app.height,0,-1,1); glMatrixMode(GL_MODELVIEW); glLoadIdentity();
            if(smoke) {
                // Exercise the same input handlers as interactive keyboard and mouse use.
                auto key=[&](SDL_Keycode code) { SDL_Event e{}; e.type=SDL_KEYDOWN; e.key.keysym.sym=code; app.event(e); };
                if(frame==1) key(SDLK_SPACE);
                if(frame==2) {
                    app.click({170,400}); app.click({170,218});
                    app.click({30,330}); SDL_Event e{}; e.type=SDL_TEXTINPUT;
                    SDL_strlcpy(e.text.text,"0.08",sizeof(e.text.text)); app.event(e); key(SDLK_RETURN); key(SDLK_g); key(SDLK_SPACE);
                    if(!app.graph_decimal || !app.solved || app.graph.edges.size()!=11) throw std::runtime_error("UI smoke test failed");
                }
            }
            app.draw();
            if(smoke) {
                screenshot(std::string(argv[2])+"-"+std::to_string(frame)+".ppm",w,h);
                if(glGetError()!=GL_NO_ERROR) throw std::runtime_error("OpenGL render error");
                if(++frame==3) { std::cout<<"OpenGL UI smoke test passed (integer, flow, decimal density).\n"; running=false; }
            }
            SDL_GL_SwapWindow(window); SDL_Delay(16);
        }
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; exit_code=1; }
    SDL_GL_DeleteContext(context); SDL_DestroyWindow(window); SDL_Quit(); return exit_code;
}
