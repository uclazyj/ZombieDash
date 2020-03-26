#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw

#include "Actor.h"
//#include "GameWorld.h"
#include "Level.h" // required to use our provided class
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

// contains your StudentWorld class implementation

StudentWorld::StudentWorld(string assetPath)
:GameWorld(assetPath),m_penelope(nullptr),m_finishCurrentLevel(false),m_nVaccineGoodies(0),m_nCharges(0),m_nLandmines(0),m_nInfection(0),m_nCitizens(0)
{
}

int StudentWorld::init()
{
    if(loadLevelFile() == Level::load_fail_file_not_found)
    {
        return GWSTATUS_PLAYER_WON;
    }

    else if(loadLevelFile() == Level::load_fail_bad_format)
    {
        return GWSTATUS_LEVEL_ERROR;
    }
    
    for(int level_x = 0;level_x < LEVEL_WIDTH; level_x++)
        for(int level_y = 0;level_y < LEVEL_HEIGHT; level_y++)
        {
            if (loadMazeEntry(level_x, level_y) == Level::player && m_penelope == nullptr)
                m_penelope = new Penelope(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y);
            else if(loadMazeEntry(level_x, level_y) == Level::wall)
                m_pointerList.push_back(new Wall(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
            else if(loadMazeEntry(level_x, level_y) == Level::exit)
                m_pointerList.push_back(new Exit(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
            else if(loadMazeEntry(level_x, level_y) == Level::pit)
                m_pointerList.push_back(new Pit(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
            else if(loadMazeEntry(level_x, level_y) == Level::vaccine_goodie)
                m_pointerList.push_back(new VaccineGoodie(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
            else if(loadMazeEntry(level_x, level_y) == Level::gas_can_goodie)
                m_pointerList.push_back(new GasCanGoodie(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
            else if(loadMazeEntry(level_x, level_y) == Level::landmine_goodie)
                m_pointerList.push_back(new LandmineGoodie(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
            else if(loadMazeEntry(level_x, level_y) == Level::dumb_zombie)
                m_pointerList.push_back(new DumbZombie(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
            else if(loadMazeEntry(level_x, level_y) == Level::smart_zombie)
                m_pointerList.push_back(new SmartZombie(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
            else if(loadMazeEntry(level_x, level_y) == Level::citizen)
            {
                m_pointerList.push_back(new Citizen(this,SPRITE_WIDTH * level_x,SPRITE_HEIGHT * level_y));
                m_nCitizens++;
            }
            
        }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if(!getPenelope()->isAlive())
    {
        decLives();
        reset();
        return GWSTATUS_PLAYER_DIED;
    }
    else
        m_penelope -> doSomething();
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end(); p++)
    {
        if((*p)->isAlive())
           (*p)->doSomething();
    }
    
    if(m_finishCurrentLevel) // The penelope walks into an exit
    {
        playSound(SOUND_LEVEL_FINISHED);
        reset();
        m_finishCurrentLevel = false; // Reset this to false for the upcoming new level
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    // Delete the dead objects
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();)
    {
        if(!((*p)->isAlive()))
        {
            delete *p;
            p = m_pointerList.erase(p);
        }
        else
            ++p;
    }
    
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(6)<<getScore() << "  Level: " << getLevel() << "  Lives: "<< getLives() << "  Vacc: "<< getNVaccineGoodies() << "  Flames: "<< getNCharges()<<"  Mines: "<<getNLandmines()<<"  Infected: "<<getNInfection();
    string gameStatText = oss.str();
    setGameStatText(gameStatText);

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (m_penelope != nullptr)
    {
        delete m_penelope;
        m_penelope = nullptr;
    }
    if(m_pointerList.size()!=0)
    {
        for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end(); p++)
        delete *p;
        list<Actor*> emptyList;     // question: is this a good way to delete the m_pointerList?
        m_pointerList = emptyList;  // So we don't have to worry about those dangling pointers?
        
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}



bool StudentWorld::isOverlap(int x1, int y1, int x2, int y2)
{
    int dSquare = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
    if(dSquare <= 100)
        return true;
    else
        return false;
}

bool StudentWorld::isAway(int x1, int y1, int x2, int y2)
{
    if (abs(x1-x2) >= SPRITE_WIDTH || abs(y1-y2) >= SPRITE_HEIGHT
        // For convenience, the object is away from itself,
        // and is also away from the adjacient position
        // Since the Penelope moves the fastest with a speed of 4
        // (Citizen:2, zombies:1), we will use 4 in the following line
       || ((abs(x1-x2) <= 4) && (abs(y1-y2) <= 4)))
        
        return true;
    else
        return false;
}

bool StudentWorld::isReallyAway(int x1, int y1, int x2, int y2)
{
    if (abs(x1-x2) >= SPRITE_WIDTH || abs(y1-y2) >= SPRITE_HEIGHT)
        return true;
    else
        return false;
}

bool StudentWorld::isEmpty(int x,int y)
{
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end(); p++)
    {
        if (isOverlap(x, y, (*p)->getX(), (*p)->getY()))
            return false;
    }
    return true;
}

bool StudentWorld::canBeThere(int x,int y)
{

    if(x < 0 || x > VIEW_WIDTH || y < 0 || y > VIEW_HEIGHT)
        return false;
    if(!isAway(x, y, getPenelope()->getX(), getPenelope()->getY()))
        return false;
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end(); p++)
    {
        if((*p)->canBlock())
            if(!isAway(x, y, (*p)->getX(), (*p)->getY()))
                return false;
    }
    return true;
}

bool StudentWorld::canFireThere(int x,int y)
{
    
    if(x < 0 || x > VIEW_WIDTH || y < 0 || y > VIEW_HEIGHT)
        return false;
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end(); p++)
    {
            if(!isReallyAway(x, y, (*p)->getX(), (*p)->getY()) && ((*p)->isWall()))
                return false;
    }
    return true;
}

void StudentWorld::fireAllFireableActors(int xFire,int yFire)
{
    if(isOverlap(xFire, yFire, getPenelope()->getX(), getPenelope()->getY()))
    {
        getPenelope()->goDie();
    }

    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
        if(isOverlap(xFire, yFire, (*p)->getX(), (*p)->getY()))
            if((*p)->canBeFired() && (*p)->isAlive())
            {
                (*p)->goDie();
                if((*p)->isDumbZombie())
                {
                    increaseScore(1000);
                    playSound(SOUND_ZOMBIE_DIE);
                    if (randInt(1, 10) == 1)
                    {
                        int vaccinePosition = randInt(1, 4);
                        switch (vaccinePosition)
                        {
                            case 1:
                                if(isEmpty((*p)->getX()+SPRITE_WIDTH, (*p)->getY()))
                                    addActor(new VaccineGoodie(this,(*p)->getX()+SPRITE_WIDTH,(*p)->getY()));
                                break;
                            case 2:
                                if(isEmpty((*p)->getX()-SPRITE_WIDTH, (*p)->getY()))
                                    addActor(new VaccineGoodie(this,(*p)->getX()-SPRITE_WIDTH,(*p)->getY()));
                                break;
                            case 3:
                                if(isEmpty((*p)->getX(), (*p)->getY()+SPRITE_HEIGHT))
                                    addActor(new VaccineGoodie(this,(*p)->getX(),(*p)->getY()+SPRITE_HEIGHT));
                                break;
                            case 4:
                                if(isEmpty((*p)->getX(), (*p)->getY()-SPRITE_HEIGHT))
                                    addActor(new VaccineGoodie(this,(*p)->getX(),(*p)->getY()-SPRITE_HEIGHT));
                            default:
                                break;
                        }
                        addActor(new VaccineGoodie(this,(*p)->getX(),(*p)->getY()));
                    }
                }
                else if((*p)->isSmartZombie())
                {
                    increaseScore(2000);
                    playSound(SOUND_ZOMBIE_DIE);
                }
                else if((*p)->isCitizen())
                {
                    increaseScore(-1000);
                    playSound(SOUND_CITIZEN_DIE);
                    m_nCitizens--;
                }
                else if((*p)->isLandmine())
                {
                    explode((*p)->getX(), (*p)->getY());
                }
                    
            }
}

void StudentWorld::explode(int x, int y)
{
    playSound(SOUND_LANDMINE_EXPLODE);
    addActor(new Pit(this,x,y));
    for(int i = -1; i <= 1; i++)
        for(int j = -1; j <= 1; j++)
            if(canFireThere(x+i*SPRITE_WIDTH,y+j*SPRITE_HEIGHT))
                addActor(new Flame(this,x+i*SPRITE_WIDTH,y+j*SPRITE_HEIGHT,Actor::up));
}

void StudentWorld::infectAllInfectableCitizens(int xVomit, int yVomit)
{
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
        if(isOverlap(xVomit, yVomit, (*p)->getX(), (*p)->getY()))
            if((*p)->isCitizen() && (*p)->isAlive())
                (*p)->getInfected();
}

void StudentWorld::trapAlltrapableActors(int xPit, int yPit)
{
    if(isOverlap(xPit, yPit, getPenelope()->getX(), getPenelope()->getY()))
        getPenelope()->goDie();
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
        if(isOverlap(xPit, yPit, (*p)->getX(), (*p)->getY()))
            if((*p)->isDumbZombie())
            {
                (*p)->goDie();
                playSound(SOUND_ZOMBIE_DIE);
                increaseScore(1000);
            }
            else if((*p)->isSmartZombie())
            {
                (*p)->goDie();
                playSound(SOUND_ZOMBIE_DIE);
                increaseScore(2000);
            }
            else if((*p)->isCitizen())
            {
                (*p)->goDie();
                m_nCitizens--;
                playSound(SOUND_CITIZEN_DIE);
                increaseScore(-1000);
            }
}

void StudentWorld::exitAllCitizens(int xExit, int yExit)
{
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
        if(isOverlap(xExit, yExit, (*p)->getX(), (*p)->getY()))
            if((*p)->isCitizen())
            {
                increaseScore(500);
                (*p)->goDie();
                playSound(SOUND_CITIZEN_SAVED);
                m_nCitizens--;
            }
}

bool StudentWorld::anythingStepOnLandmine(int xLandmine, int yLandmine)
{
    if(isOverlap(xLandmine, yLandmine, getPenelope()->getX(), getPenelope()->getY()))
        return true;
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
        if((*p)->isCitizen() || (*p)->isDumbZombie()||(*p)->isSmartZombie()||(*p)->isFlame())
            if(isOverlap(xLandmine, yLandmine, (*p)->getX(), (*p)->getY()))
                return true;
    return false;
}

//bool StudentWorld::flameTriggerLandmine(int xLandmine, int yLandmine)
//{
//    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
//        if((*p)->isFlame())
//            if(isOverlap(xLandmine, yLandmine, (*p)->getX(), (*p)->getY()))
//                return true;
//    return false;
//}



int StudentWorld::getVomitCoordinateX(int xZombie, int yZombie, Direction dirZombie)
{
    switch (dirZombie) {
        case Actor::left:
            return xZombie-SPRITE_WIDTH;
            break;
        case Actor::right:
            return xZombie+SPRITE_WIDTH;
            break;
        case Actor::up:
            return xZombie;
            break;
        case Actor::down:
            return xZombie;
            break;
        default:
            return -1;
            break;
    }
}

int StudentWorld::getVomitCoordinateY(int xZombie, int yZombie, Direction dirZombie)
{
    switch (dirZombie) {
        case Actor::left:
            return yZombie;
            break;
        case Actor::right:
            return yZombie;
            break;
        case Actor::up:
            return yZombie+SPRITE_HEIGHT;
            break;
        case Actor::down:
            return yZombie-SPRITE_HEIGHT;
            break;
        default:
            return -1;
            break;
    }
}

void StudentWorld::vomitAPerson(int xZombie, int yZombie, Direction dirZombie)
{
    // Compute the vomit coordinates
    int xVomit = getVomitCoordinateX(xZombie, yZombie, dirZombie);
    int yVomit = getVomitCoordinateY(xZombie, yZombie, dirZombie);
    // Vomit a citizen
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
        if((*p)->isCitizen())
            switch (dirZombie)
            {
                case Actor::left:
                    if((*p)->getY() == yZombie && (*p)->getX() < xZombie)
                        if(isOverlap((*p)->getX(), (*p)->getY(), xVomit, yVomit) && randInt(1, 3) == 1)
                        {
                            addActor(new Vomit(this,xVomit,yVomit,dirZombie));
                            playSound(SOUND_ZOMBIE_VOMIT);
                            (*p)->getInfected();
                            return;
                        }
                    break;
                case Actor::right:
                    if((*p)->getY() == yZombie && (*p)->getX() > xZombie)
                        if(isOverlap((*p)->getX(), (*p)->getY(), xVomit, yVomit) && randInt(1, 3) == 1)
                        {
                            addActor(new Vomit(this,xVomit,yVomit,dirZombie));
                            playSound(SOUND_ZOMBIE_VOMIT);
                            (*p)->getInfected();
                            return;
                        }
                    break;
                case Actor::up:
                    if((*p)->getY() > yZombie && (*p)->getX() == xZombie)
                        if(isOverlap((*p)->getX(), (*p)->getY(), xVomit, yVomit) && randInt(1, 3) == 1)
                        {
                            addActor(new Vomit(this,xVomit,yVomit,dirZombie));
                            playSound(SOUND_ZOMBIE_VOMIT);
                            (*p)->getInfected();
                            return;
                        }
                    break;
                case Actor::down:
                    if((*p)->getY() < yZombie && (*p)->getX() == xZombie)
                        if(isOverlap((*p)->getX(), (*p)->getY(), xVomit, yVomit) && randInt(1, 3) == 1)
                        {
                            addActor(new Vomit(this,xVomit,yVomit,dirZombie));
                            playSound(SOUND_ZOMBIE_VOMIT);
                            (*p)->getInfected();
                            return;
                        }
                    break;
                default:
                    break;
            }
    // Vomit a penelope
    switch (dirZombie)
    {
        case Actor::left:
            if(getPenelope()->getY() == yZombie && getPenelope()->getX() < xZombie)
                if(isOverlap(getPenelope()->getX(), getPenelope()->getY(), xVomit, yVomit) && randInt(1, 3) == 1)
                {
                    addActor(new Vomit(this,xVomit,yVomit,dirZombie));
                    playSound(SOUND_ZOMBIE_VOMIT);
                    getPenelope()->getInfected();
                    return;
                }
            break;
        case Actor::right:
            if(getPenelope()->getY() == yZombie && getPenelope()->getX() > xZombie)
                if(isOverlap(getPenelope()->getX(), getPenelope()->getY(), xVomit, yVomit) && randInt(1, 3) == 1)
                {
                    addActor(new Vomit(this,xVomit,yVomit,dirZombie));
                    playSound(SOUND_ZOMBIE_VOMIT);
                    getPenelope()->getInfected();
                    return;
                }
            break;
        case Actor::up:
            if(getPenelope()->getY() > yZombie && getPenelope()->getX() == xZombie)
                if(isOverlap(getPenelope()->getX(), getPenelope()->getY(), xVomit, yVomit) && randInt(1, 3) == 1)
                {
                    addActor(new Vomit(this,xVomit,yVomit,dirZombie));
                    playSound(SOUND_ZOMBIE_VOMIT);
                    getPenelope()->getInfected();
                    return;
                }
            break;
        case Actor::down:
            if(getPenelope()->getY() < yZombie && getPenelope()->getX() == xZombie)
                if(isOverlap(getPenelope()->getX(), getPenelope()->getY(), xVomit, yVomit) && randInt(1, 3) == 1)
                {
                    addActor(new Vomit(this,xVomit,yVomit,dirZombie));
                    playSound(SOUND_ZOMBIE_VOMIT);
                    getPenelope()->getInfected();
                    return;
                }
            break;
        default:
            break;
    }
    
}


Actor* StudentWorld::getClosestPerson(int xZombie, int yZombie)
{
    Actor* closestPerson = getPenelope();
    int closestDistanceSquare = distSquare(xZombie, yZombie, getPenelope()->getX(), getPenelope()->getY());
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
        if((*p)->isCitizen())
        {
            int d2 = distSquare(xZombie, yZombie, (*p)->getX(), (*p)->getY());
            if(d2 < closestDistanceSquare)
            {
                closestDistanceSquare = d2;
                closestPerson = (*p);
            }
        }
    
    return closestPerson;
}

Actor* StudentWorld::getClosestZombie(int xCitizen, int yCitizen)
{
    Actor* closestZombie = nullptr;
    int closestDistanceSquare = VIEW_WIDTH * VIEW_WIDTH + VIEW_HEIGHT * VIEW_HEIGHT;
    for (list<Actor*>::iterator p = m_pointerList.begin(); p != m_pointerList.end();p++)
        if((*p)->isDumbZombie() || (*p) -> isSmartZombie())
        {
            int d2 = distSquare(xCitizen, yCitizen, (*p)->getX(), (*p)->getY());
            if(d2 < closestDistanceSquare)
            {
                closestDistanceSquare = d2;
                closestZombie = (*p);
            }
        }
    return closestZombie;
}


Level::LoadResult StudentWorld::loadLevelFile()
{
    Level lev(assetPath());

    ostringstream oss;
    oss << "level0" << getLevel() << ".txt";
    string levelFile = oss.str();

    Level::LoadResult result = lev.loadLevel(levelFile);
    return result;
}


Level::MazeEntry StudentWorld::loadMazeEntry(int level_x, int level_y)
{
    Level lev(assetPath());
    
    // Comment out the following lines will lead to a bug, but I am not sure why...
    ostringstream oss;
    oss << "level0" << getLevel() << ".txt";
    string levelFile = oss.str();

    Level::LoadResult result = lev.loadLevel(levelFile);
    
        if (result == Level::load_success)
    {
         cerr << "Successfully loaded level " << getLevel() << endl;
    
        Level::MazeEntry ge = lev.getContentsOf(level_x,level_y); // level_x=5, level_y=10

        return ge;
    }
    return Level::empty;
}
