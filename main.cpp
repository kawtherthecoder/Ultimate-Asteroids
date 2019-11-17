///////////////////////////////////////////////////
// ULTIMATE ASTEROIDS GAME WITH SFML
// PROGRAMMER: KAWTHER ROUABHI
// DATE: APRIL 7, 2019
// DESCRIPTION: This asteroids game uses polymorphism and inheritance concepts in its program. With a total of five levels increasing in difficulty and a limit of three lives, players are invited to journey through the galaxy.
///////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <list>
#include <cmath>

//////////////CITATIONS///////////////
// UFO IMAGE: "UFO" BY AHA-SOFT (ICONARCHIVE.COM) - FREE FOR PERSONAL USE
// MUSIC: "STAR COMMANDER1" BY DL-SOUNDS (DL-SOUNDS.COM) - FREE FOR PERSONAL USE
// ASTEROID KILLED SOUND: "ROBOT, GLITCH, STUTTER" BY LESSER VIBES (ZAPSPLAT.COM) - FREE FOR NONCOMMERCIAL USE
// UFO SOUND: "MENACING PAD REMINISCENT OF AN ALIEN INVASION" BY SUMO BLANCO (ZAPSPLAT.COM) - FREE FOR NONCOMMERICIAL USE
// LIFE ICON: "HEARTS FREE ICON" (FLATICON.COM) - FREE FOR PERSONAL USE
// FONT: "VIDEOPHREAK" BY PIZZADUDE (1001FREEFONTS.COM) - FREE FOR PERSONAL USE
// RED BACKGROUND: "STARS GALAXY CLUSTER" BY GERD ALTMANN (PIXABAY.COM) - FREE FOR PERSONAL USE
// BACKGROUND 1: "GALAXY STARS" BY LARISA KOSHKINA (PIXABAY.COM) - FREE FOR PERSONAL USE

using namespace sf;

const int W = 1200; // height and width of app
const int H = 800;

float DEGTORAD = 0.017453f; // conversion to radians

class Animation {
public:

    float Frame, speed;
    Sprite sprite;
    std::vector<IntRect> frames;

    Animation() {}

    Animation(Texture &t, int x, int y, int w, int h, int count, float Speed) {
        Frame = 0; // number of frames
        speed = Speed; // speed at which the frames of the animation are displayed

        for (int i = 0; i < count; i++) // for each frame
            frames.push_back(IntRect(x + i * w, y, w, h)); //set dimensions

        sprite.setTexture(t); // graphics set up
        sprite.setOrigin(w / 2, h / 2);
        sprite.setTextureRect(frames[0]);
    }


    void update() {
        Frame += speed; // changes frame for each speed increment
        int n = frames.size();
        if (Frame >= n) Frame -= n;
        if (n > 0) sprite.setTextureRect(frames[int(Frame)]); // creates moving animation effect
    }

    bool isEnd() {
        return Frame + speed >= frames.size(); // is end of animation
    }

};


class Entity {
public:
    float x, y, dx, dy, R, angle; // attributes of an entity (ie. asteroid, spaceship, ufo, bullet)
    bool life; // whether or not the entity should be displayed
    std::string name;
    Animation anim; // its animation if applicable

    Entity() {
        life = 1;
    }

    void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1) { // function sort of like a constructor giving entity needed values
        anim = a;
        x = X;
        y = Y;
        angle = Angle;
        R = radius;
    }

    virtual void update() {}; // polymorphic function that is used in derived classes because each has its own behavior

    void draw(RenderWindow &app) { // displays an entity or sprite or text (etc.) to screen
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle + 90);
        app.draw(anim.sprite);

        CircleShape circle(R);
        circle.setFillColor(Color(255, 0, 0, 170));
        circle.setPosition(x, y);
        circle.setOrigin(R, R);
        //app.draw(circle);
    }

    virtual ~Entity() {}; // polymorphic destructor to kill entities based on what they are
};

///////////UFO class documentation///////////
class ufo : public Entity {
    static int count; // updates at run-time for numer of UFOs alive
public:
    ufo() {
        dx = 2 + rand() % 4; // change in position
        dy = 0;
        count++;
        angle = 0;
        name = "ufo";
    }

    ~ufo() //destructor
    {
        count--;
    }

    void update() {
        x += dx;
        y += dy;
    }

    static unsigned int getCount() // gets count to keep at no more than one
    {
        return count;
    }
};

int ufo::count = 0;

class asteroid : public Entity {
    static int count; // updates at run-time for current asteroid count
public:
    asteroid() {
        dx = rand() % 8 - 4; // change in position
        dy = rand() % 8 - 4;
        count++;
        name = "asteroid";
    }

    ~asteroid() //destructor
    {
        count--;
    }

    void update() {
        x += dx; // change position by dx and dy
        y += dy;

        if (x > W) x = 0; // wrap around to other side of screen
        if (x < 0) x = W;
        if (y > H) y = 0;
        if (y < 0) y = H;
    }

    static unsigned int getCount()
    {
        return count;
    }

};

int asteroid::count = 0;

class bullet : public Entity {
public:
    bullet() {
        name = "bullet";
    }

    void update() {
        dx = cos(angle * DEGTORAD) * 6; //change in position
        dy = sin(angle * DEGTORAD) * 6;
        // angle+=rand()%6-3;
        x += dx;
        y += dy;

        if (x > W || x < 0 || y > H || y < 0) life = 0; // disappear if falls off screen
    }

};


class player : public Entity // player = the spaceship
{
public:
    bool thrust; // whether or not forward key is pressed -- trust = speeding up/down effect

    player() {
        name = "player";
    }

    void update() {
        if (thrust) { // trust is for the effect of making the ship speed up and slow down depending on if the up key is being pressed
            dx += cos(angle * DEGTORAD) * 0.2;
            dy += sin(angle * DEGTORAD) * 0.2;
        } else {
            dx *= 0.99;
            dy *= 0.99;
        }

        int maxSpeed = 15;
        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed) {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }

        x += dx; // updating positon by dy and dx
        y += dy;

        if (x > W) x = 0; // wrap around screen
        if (x < 0) x = W;
        if (y > H) y = 0;
        if (y < 0) y = H;
    }

};


bool isCollide(Entity *a, Entity *b) {
    return (b->x - a->x) * (b->x - a->x) +
           (b->y - a->y) * (b->y - a->y) <
           (a->R + b->R) * (a->R + b->R);
}


int main() {
    srand(time(0));

    RenderWindow app(VideoMode(W, H), "Asteroids!"); // title of window
    app.setFramerateLimit(60); // game runs at 60 frames per second

    Texture t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12;
    t1.loadFromFile("images/spaceship.png"); //loading pictures from images folder into textures to be used for sprites and animations
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_red.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");
    t8.loadFromFile("images/UFO.png");
    t9.loadFromFile("images/lifeicon.png");
    t10.loadFromFile("images/lifeicon.png");
    t11.loadFromFile("images/lifeicon.png");
    t12.loadFromFile("images/finallevelback.jpg");

    t1.setSmooth(true); // makes pictures smooth
    t2.setSmooth(true);
    t8.setSmooth(true);

    sf::Vector2u TextureSize; // scaling new background image to fit game screen
    sf::Vector2u WindowSize;
    TextureSize = t2.getSize();
    WindowSize = app.getSize();
    float ScaleX = (float) WindowSize.x / TextureSize.x;
    float ScaleY = (float) WindowSize.y / TextureSize.y;

    // SOUNDS LOADING
    sf::Music music;
    if (!music.openFromFile("sounds/JustDanceLadyGaGa.flac")) {
        //std::cout << "Unable to open StarCommander1.wav";
        return EXIT_FAILURE;
    }
    bool loop = 1;
    music.setLoop(loop); // loops music

    sf::SoundBuffer astbuff;
    if (!astbuff.loadFromFile("sounds/robotglitch.ogg")) { // soundbuffer for asteroid explosion
        return EXIT_FAILURE;
    }
    sf::Sound astsound;
    astsound.setBuffer(astbuff); // asteroid explosion noise

    sf::SoundBuffer ufobuff;
    if (!ufobuff.loadFromFile("sounds/ufosound.ogg")) // ufo sound and buffer
    {
        return EXIT_FAILURE;
    }
    sf::Sound ufosound;
    ufosound.setBuffer(ufobuff);

    Sprite background(t2); // sets loaded picture file as background sprite
    background.setScale(ScaleX,ScaleY); // set scale of image
    Sprite finalback(t12);

    Sprite life1(t9); //life icon images
    Sprite life2(t10);
    Sprite life3(t11);
    life1.setPosition(sf::Vector2f(1125.f, 10.f)); //set position of life icons
    life2.setPosition(sf::Vector2f(1050.f, 10.f));
    life3.setPosition(sf::Vector2f(975.f, 10.f));


    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5); // creates animation objects for entities and sets texture, speed, etc.
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
    Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);
    Animation sUFO(t8, 0, 0, 70, 70, 1, 0.1); // ufo animation

    // text settings for score and level

    Font scorefont;
    if (!scorefont.loadFromFile("fonts/VideoPhreak.ttf"))
    {
        return EXIT_FAILURE;
    }

    Text scoretext; // text item that displays the current score
    scoretext.setFont(scorefont);
    unsigned int score = 0;
    scoretext.setString("SCORE 0");
    scoretext.setFillColor(sf::Color::Cyan);
    scoretext.setCharacterSize(64);
    scoretext.setPosition(10.f,10.f);

    Font levelfont;
    if (!levelfont.loadFromFile("fonts/VideoPhreak.ttf"))
    {
        return EXIT_FAILURE;
    }
    unsigned int level = 1;
    Text leveltext; // text item that displays the current level
    leveltext.setFont(levelfont);
    leveltext.setString("LEVEL 1");
    leveltext.setFillColor(sf::Color::Yellow);
    leveltext.setCharacterSize(50);
    leveltext.setPosition(960.f, 700.f);

    Text gameover; // gameover displayed when lives = 0
    gameover.setFont(levelfont);
    gameover.setString("GAME OVER");
    gameover.setFillColor(sf::Color::Red);
    gameover.setCharacterSize(110);
    gameover.setPosition(275.f,310.f);

    std::list<Entity *> entities;

    music.play();

    for (int i = 0; i < 15; i++) {
        asteroid *a = new asteroid();
        a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
        entities.push_back(a);
    }

    player *p = new player();
    p->settings(sPlayer, 200, 200, 0, 20);
    entities.push_back(p);

    double time = 0; // measures seconds passed in game assuming frame rate = 60/s
    int lives = 3;

    /////main loop/////
    while (app.isOpen()) {

        scoretext.setString("SCORE "+std::to_string(score));
        leveltext.setString("LEVEL "+std::to_string(level));

        Event event; // events are anything done by the human on keyboard or mouse (clicking or typing a key)
        while (app.pollEvent(event)) {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space) {
                    bullet *b = new bullet();
                    b->settings(sBullet, p->x, p->y, p->angle, 10);
                    entities.push_back(b);
                }
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3; // sets game control keys
        if (Keyboard::isKeyPressed(Keyboard::Left)) p->angle -= 3;
        if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
        else p->thrust = false;


        for (auto a:entities) // explosions display -- goes through all entities on screen to check and see if any/which collisions are happening
            for (auto b:entities) {
                if (a->name == "asteroid" && b->name == "bullet")
                    if (isCollide(a, b)) {
                        a->life = false;
                        b->life = false;

                        Entity *e = new Entity();
                        e->settings(sExplosion, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);

                        astsound.play();
                        score += 33; // 33 points added to score for shooting an asteroid

                        for (int i = 0; i < 2; i++) {
                            if (a->R == 15) continue;
                            Entity *e = new asteroid();
                            e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
                            entities.push_back(e);
                        }

                    }

                if (a->name == "player" && b->name == "asteroid") // asteroid/player collision
                    if (isCollide(a, b)) {
                        b->life = false;

                        Entity *e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e); // adds new explosion to entities list to be displayed

                        if (score >=15)
                            score -= 15; // 15 points lost for hitting asteroid
                        else
                            score = 0;
                        p->settings(sPlayer, W / 2, H / 2, 0, 20);
                        p->dx = 0; // resets player to the center of screen ****** LIFE CODE *******
                        p->dy = 0;

                        //removing life icon when lost
                        if (lives == 3)
                        {
                            life3.setColor(sf::Color::Transparent);
                        }
                        else if (lives == 2)
                        {
                            life2.setColor(sf::Color::Transparent);

                        }
                        else
                        {
                            life1.setColor(sf::Color::Transparent);
                        }
                        lives--;
                    }

                if (a->name == "player" && b->name == "ufo") // ufo/player collision
                    if (isCollide(a, b)) {
                        b->life = false;

                        Entity *e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e); // adds new explosion to entities list to be displayed

                        if (score >= 20)
                            score -= 20; // 20 points lost for hitting ufo
                        else
                            score = 0;

                        ufosound.pause();

                        p->settings(sPlayer, W / 2, H / 2, 0, 20);
                        p->dx = 0; // resets player to the center of screen ****** LIFE CODE *******
                        p->dy = 0;

                        // displaying lives icons/removing when life is lost
                        if (lives == 3)
                        {
                            life3.setColor(sf::Color::Transparent);
                        }
                        else if (lives == 2)
                        {
                            life2.setColor(sf::Color::Transparent);

                        }
                        else
                        {
                            life1.setColor(sf::Color::Transparent);
                        }
                        lives--;
                    }

                if (a->name == "ufo" && b->name == "bullet") // ufo/bullet collision
                    if (isCollide(a, b)) {
                        a->life = false; // scheduled to be deleted
                        b->life = false;

                        Entity *e = new Entity(); // adds explosion to entities list to be displayed
                        e->settings(sExplosion, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);

                        score += 75; // 75 points for shooting a ufo

                        ufosound.pause();

//                        for (int i = 0; i < 2; i++) {
//                            if (a->R == 15) continue;
//                            Entity *e = new asteroid();
//                            e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
//                            entities.push_back(e);
//                        }

                    }
            }


        if (p->thrust) p->anim = sPlayer_go; // go animation used when up key is pressed to move forward
        else p->anim = sPlayer;


        for (auto e:entities)
            if (e->name == "explosion")
                if (e->anim.isEnd()) e->life = 0;

//        if (rand() % 150 == 0) // PART 3 ?? CREATES NEW ASTEROIDS
//        {
//            asteroid *a = new asteroid();
//            a->settings(sRock, 0, rand() % H, rand() % 360, 25);
//            entities.push_back(a);
//        }

        if (asteroid::getCount() == 0) // increasing the difficulty of each level by adding more and more asteroids each level
        {
            if (level != 5) {
                level++;
            }
            if (level == 2) {
                for (int i = 0; i < 15; i++) { // calling asteroid constructor
                    asteroid *a = new asteroid();
                    a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
                    entities.push_back(a);
                }
            }
            if (level == 3)
            {
                for (int i = 0; i < 25; i++) {
                    asteroid *a = new asteroid();
                    a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
                    entities.push_back(a);
                }
            }
            if (level == 4)
            {
                for (int i = 0; i < 34; i++) {
                    asteroid *a = new asteroid();
                    a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
                    entities.push_back(a);
                }
            }
            if (level == 5) // final level
            {
                for (int i = 0; i < 45; i++) {
                    asteroid *a = new asteroid();
                    a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
                    entities.push_back(a);
                }
            }
        }

        // ****** NEED SIMILAR CODE TO ADD UFO TO ENTITIES LIST

        //use static count for ufo to make sure only one ufo is on the screen at a time
            if (rand() % 100 == 25 && ufo::getCount() == 0) {
                ufo *u = new ufo();
                u->settings(sUFO, 0, rand() % H, 270, 40);
                entities.push_back(u);
                ufosound.play();
            }

            if (ufo::getCount() == 1)
            {
                // && no longer on screen { ufosound.pause(); }
            }


        for (auto i = entities.begin(); i != entities.end();) {
            Entity *e = *i;

            e->update(); // dynamically finds out which update() derived class to use based on type of e
            e->anim.update();

            if (e->life == false) {
                i = entities.erase(i);
                delete e;
            } // if entity e has been scheduled by update() to be deleted, destructor is called and e is deleted
            else i++;
        }

        //////draw//////

        if (level < 5) {
            app.draw(background); //draw creates the pictures, but does not display yet
        }
        else {
            app.draw(finalback);
        }
        if (lives > 0)
        {
            app.draw(scoretext); // draw stuff necessary for the game
            app.draw(leveltext);
            app.draw(life1);
            app.draw(life2);
            app.draw(life3);
        }
        else {
            app.draw(gameover);
            app.draw(scoretext);
        }

        for (auto i:entities)
            i->draw(app); // draw entities with life = 0

        app.display(); // display() displays drawn entities

        //time += static_cast<double>(1)/60;
    }

    return 0;
}
