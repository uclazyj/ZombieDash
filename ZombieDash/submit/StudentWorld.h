#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

#include "Actor.h"
#include "Level.h"

using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

// contains your StudentWorld class declaration

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    Penelope* getPenelope()
    {
        return m_penelope;
    }
    int distSquare(int x1, int y1, int x2, int y2)
    {
        return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
    }
    
    bool isOverlap(int x1, int y1, int x2, int y2);
    bool isAway(int x1, int y1, int x2, int y2);
    bool isEmpty(int x,int y);
    bool canBeThere(int x,int y);
    bool canFireThere(int x,int y);
    void fireAllFireableActors(int xFire, int yFire);
    void infectAllInfectableCitizens(int xVomit, int yVomit);
    void trapAlltrapableActors(int xPit, int yPit);
    void exitAllCitizens(int xExit, int yExit);// ToBeImplemented
    bool anythingStepOnLandmine(int xLandmine, int yLandmine);
    bool flameTriggerLandmine(int xLandmine, int yLandmine);
    
    
    int getVomitCoordinateX(int xZombie, int yZombie, Direction dirZombie);
    int getVomitCoordinateY(int xZombie, int yZombie, Direction dirZombie);
    void vomitAPerson(int xZombie, int yZombie, Direction dirZombie);
    
    Actor* getClosestPerson(int xZombie, int yZombie);
    Actor* getClosestZombie(int xCitizen, int yCitizen);
    
    
    Level::LoadResult loadLevelFile();
    Level::MazeEntry loadMazeEntry(int level_x, int level_y);
    
    
    void addActor(Actor* p)
    {
        m_pointerList.push_back(p);
    }
    
    void finishCurrentLevel()
    {
        m_finishCurrentLevel = true;
    }

    ////// Vaccine Goodie //////
    void pickUpVaccineGoodie()
    {
        m_nVaccineGoodies++;
    }
    
    int getNVaccineGoodies()
    {
        return m_nVaccineGoodies;
    }
    
    void decNVaccineGoodies()
    {
        m_nVaccineGoodies--;
    }
    
    
    
    ////// Gas Can Goodie //////
    
    void pickUpGasCanGoodie()
    {
        m_nCharges += 5;
    }
    
    int getNCharges()
    {
        return m_nCharges;
    }
    
    void decNcharges()
    {
        m_nCharges--;
    }
    
    ////// Landmine Goodie //////
    
    void pickUpLandmineGoodie()
    {
        m_nLandmines += 2;
    }
    
    
    int getNLandmines()
    {
        return m_nLandmines;
    }
    
    void decNLandmines()
    {
        m_nLandmines--;
    }
    
    ////// Infection //////
    
    int getNInfection()
    {
        return m_nInfection;
    }
    
    void incNInfection()
    {
        m_nInfection++;
    }
    
    void cureInfection()
    {
        m_nInfection = 0;
        getPenelope() -> cureInfection();
    }
    
    
    void reset()
    {
        m_nVaccineGoodies = 0;
        m_nCharges = 0;
        m_nLandmines = 0;
        m_nInfection = 0;
        m_nCitizens = 0;
    }
    
    int getNCitizens()
    {
        return m_nCitizens;
    }
    
    void decNCitizens()
    {
        m_nCitizens--;
    }
    
    
    
private:
    Penelope* m_penelope;
    list<Actor*> m_pointerList;
    bool m_finishCurrentLevel;
    
    int m_nVaccineGoodies;
    int m_nCharges;
    int m_nLandmines;
    int m_nInfection;
    int m_nCitizens;
};

#endif // STUDENTWORLD_H_
