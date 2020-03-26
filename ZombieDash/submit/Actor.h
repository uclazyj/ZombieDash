#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


// contains base, Penelope, and Wall class declarations
// as well as constants required by these classes

/////// Base class: Actor //////////
class StudentWorld;

class Actor: public GraphObject

{
public:
    Actor(StudentWorld* SWp, int imageID, double startX, double startY, Direction dir, int depth);
    virtual ~Actor();
    StudentWorld* getWorld();
    
    virtual void doSomething() = 0;
    virtual void goDie(){return;}
    virtual bool canBlock(){return false;} // Wall, Penelope, Zombies and Citizen will overide this
    // by returning true;
    virtual bool canBeFired(){return true;}
    virtual bool isLivingThings(){return false;}
    virtual bool isAlive(){return true;} // Wall, Exit and Pit will inherit this, Penelope and
                                         // LivingThings will override this with their own isAlive function
    virtual bool isDumbZombie(){return false;}
    virtual bool isSmartZombie(){return false;}
    virtual bool isCitizen(){return false;}
    virtual bool isFlame(){return false;}
    virtual bool isWall(){return false;}
    
    virtual void getInfected(){return;}
    
private:
    StudentWorld* m_studentWorld;
    
};

//////////// Penelope ////////////

class Penelope: public Actor

{
public:
    Penelope(StudentWorld* SWp,double startX, double startY);
    virtual void doSomething();
    virtual bool canBlock(){return true;}
    virtual bool isAlive(){return m_alive;}
    virtual void getInfected(){m_infectionStatus = true;}
    void cureInfection(){m_infectionStatus = false;}
    void goDie(){m_alive = false;}
    
    
private:
    bool m_alive;
    bool m_infectionStatus;
    int m_nLandmines;
    int m_nFlamethrower;
    int m_nVaccines;
};

//////////// Wall ////////////

class Wall: public Actor
{
public:
    Wall(StudentWorld* SWp,double startX, double startY);
    virtual void doSomething();
    virtual bool isWall(){return true;}
    virtual bool canBlock(){return true;}
    virtual bool canBeFired(){return false;}
};

//////////// Exit ////////////
class Exit: public Actor
{
public:
    Exit(StudentWorld* SWp,double startX, double startY);
    virtual void doSomething();

};

//////////// Pit ////////////
class Pit: public Actor
{
public:
    Pit(StudentWorld* SWp,double startX, double startY);
    virtual void doSomething();

};

//////////// Living Things ////////////
class LivingThings: public Actor
{
public:
    LivingThings(StudentWorld* SWp, int imageID, double startX, double startY, Direction dir,int depth);
    virtual ~LivingThings();
    void doSomething();
    virtual void doDiffLivingThingsDo() = 0;
    virtual bool isLivingThings(){return true;}
    virtual bool isAlive(){return m_alive;}
    virtual void goDie()
    {m_alive = false;}
private:
    
    bool m_alive;
};


//////////// Projectile ////////////

class Projectile: public LivingThings
{
public:
    Projectile(StudentWorld* SWp, int imageID, double startX, double startY, Direction dir);
    virtual ~Projectile();
    virtual void doDiffLivingThingsDo();
    virtual void doDiffProjectileDo() = 0;
    virtual bool canBeFired(){return false;}
private:
    
    int m_timeRemaining;
};

class Flame: public Projectile
{
public:
    Flame(StudentWorld* SWp,double startX, double startY,Direction dir);
    virtual bool isFlame(){return true;}
private:
    virtual void doDiffProjectileDo();
    
};

class Vomit: public Projectile
{
public:
    Vomit(StudentWorld* SWp,double startX, double startY,Direction dir);
private:
    virtual void doDiffProjectileDo();
    
};
//////////// Goodie ////////////

class Goodie: public LivingThings
{
public:
    Goodie(StudentWorld* SWp, int imageID, double startX, double startY);
    virtual ~Goodie();
    virtual void doDiffLivingThingsDo(); // Common behaviors for all types of goodies
    virtual void doDiffGoodiesDo() = 0; // Each specific type of goodies does its own thing
};

class VaccineGoodie: public Goodie
{
public:
    VaccineGoodie(StudentWorld* SWp,double startX, double startY);
private:
    virtual void doDiffGoodiesDo();
};

class GasCanGoodie: public Goodie
{
public:
    GasCanGoodie(StudentWorld* SWp,double startX, double startY);
private:
    virtual void doDiffGoodiesDo();
};

class LandmineGoodie: public Goodie
{
public:
    LandmineGoodie(StudentWorld* SWp,double startX, double startY);
private:
    virtual void doDiffGoodiesDo();
};

//////////// Zombie ////////////
class Zombie: public LivingThings
{
public:
    Zombie(StudentWorld* SWp,double startX, double startY);
    virtual ~Zombie();
    virtual bool canBlock(){return true;}
    virtual void doDiffLivingThingsDo(); // Common behaviors for both Zombies
    virtual void doDiffZombiesDo() = 0; // Each specific type of goodies does its own thing
//    int getMovePlanDistance(){return m_movePlanDistance;}
//    void decMovePlanDistance(){m_movePlanDistance--;}
private:
    bool m_paralyzed;
    int m_movePlanDistance;
};

class DumbZombie: public Zombie
{
public:
    DumbZombie(StudentWorld* SWp,double startX, double startY);
    virtual bool isDumbZombie(){return true;}
private:
    virtual void doDiffZombiesDo();
    //int m_movePlanDistance;
};

class SmartZombie: public Zombie
{
public:
    SmartZombie(StudentWorld* SWp,double startX, double startY);
    virtual bool isSmartZombie(){return true;}
private:
    virtual void doDiffZombiesDo();
    //int m_movePlanDistance;
};

class Citizen: public LivingThings
{
public:
    Citizen(StudentWorld* SWp,double startX, double startY);
    virtual bool isCitizen(){return true;}
    virtual bool canBlock(){return true;}
    virtual void doDiffLivingThingsDo();
    virtual void getInfected(){m_infectionStatus = true;}
private:
    bool m_infectionStatus;
    int m_infectionCount;
    bool m_paralyzed;
    
};

class Landmine: public LivingThings
{
public:
    Landmine(StudentWorld* SWp,double startX, double startY);
    virtual void doDiffLivingThingsDo();
private:
    void explode();
    int m_timeLeft;
    bool m_isActive;
};


#endif // ACTOR_H_
