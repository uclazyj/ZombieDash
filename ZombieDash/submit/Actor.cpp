#include "Actor.h"
#include "StudentWorld.h"
#include "GameWorld.h"

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// contains the implementation of these classes

Actor::Actor(StudentWorld* SWp,int imageID, double startX, double startY, Direction dir, int depth)
:GraphObject(imageID,startX,startY,dir,depth),m_studentWorld(SWp)
{}

Actor::~Actor()
{}

StudentWorld* Actor::getWorld()
{
    return m_studentWorld;
}

Penelope::Penelope(StudentWorld* SWp,double startX, double startY)
:Actor(SWp,IID_PLAYER,startX,startY,right,0)
,m_alive(true),m_nLandmines(0),m_nFlamethrower(0),m_nVaccines(0),m_infectionStatus(false)
{}

void Penelope::doSomething()
{
    // Check whether she is currently alive
    if(!m_alive)
        return;
    // Check whether she is infected
    if(m_infectionStatus)
    {
        getWorld()->incNInfection();
        if(getWorld()->getNInfection() >= 500) // Becomes a zombie
        {
            m_alive = false;
            getWorld()->playSound(SOUND_PLAYER_DIE);
            return;
        }
    }

    // Check to see if the player pressed a key

    int ch;
    if (getWorld()->getKey(ch))
    {
        // user hit a key during this tick!
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                setDirection(left);
                if(getWorld()->canBeThere(getX()-4, getY()))
                    moveTo(getX()-4, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if(getWorld()->canBeThere(getX()+4, getY()))
                   moveTo(getX()+4, getY());
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if(getWorld()->canBeThere(getX(), getY()+4))
                    moveTo(getX(), getY()+4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if(getWorld()->canBeThere(getX(), getY()-4))
                    moveTo(getX(), getY()-4);
                break;
            case KEY_PRESS_ENTER:
                if(getWorld()->getNVaccineGoodies() > 0)
                {
                    getWorld()->cureInfection();
                    getWorld()->decNVaccineGoodies();
                }
                break;
            case KEY_PRESS_TAB:
                if(getWorld()->getNLandmines() > 0)
                {
                    getWorld()->decNLandmines();
                    getWorld()->addActor(new Landmine(getWorld(),getX(),getY()));
                }
                break;
            case KEY_PRESS_SPACE:
                if(getWorld()->getNCharges() > 0)
                {
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    getWorld()->decNcharges();
                    bool isFlameBlocked = false;
                    for(int i = 1;i<=3;i++)
                    {
                        if (isFlameBlocked)
                            break;
                        switch (getDirection())
                        {
                            case up:
                                if(getWorld()->canFireThere(getX(), getY()+i * SPRITE_HEIGHT))
                                    getWorld()->addActor(new Flame(getWorld(),getX(),getY()+i * SPRITE_HEIGHT,up));
                                else
                                    isFlameBlocked = true;
                                break;
                            case down:
                                if(getWorld()->canFireThere(getX(), getY()-i * SPRITE_HEIGHT))
                                    getWorld()->addActor(new Flame(getWorld(),getX(),getY()-i * SPRITE_HEIGHT,down));
                                else
                                    isFlameBlocked = true;
                                break;
                            case left:
                                if(getWorld()->canFireThere(getX()-i * SPRITE_WIDTH,getY()))
                                    getWorld()->addActor(new Flame(getWorld(),getX()-i * SPRITE_WIDTH,getY(),left));
                                else
                                    isFlameBlocked = true;
                                break;
                            case right:
                                if(getWorld()->canFireThere(getX()+i * SPRITE_WIDTH,getY()))
                                    getWorld()->addActor(new Flame(getWorld(),getX()+i * SPRITE_WIDTH,getY(),right));
                                else
                                    isFlameBlocked = true;
                                break;
                            default:
                                break;
                        }
                        
                    }
                }
        }
    }
    return;
}


Wall::Wall(StudentWorld* SWp,double startX, double startY)
:Actor(SWp,IID_WALL,startX,startY,right,0)
{}

void Wall::doSomething()
{
    return;
}


Exit::Exit(StudentWorld* SWp,double startX, double startY)
:Actor(SWp,IID_EXIT,startX,startY,right,1)
{}

void Exit::doSomething()
{
    getWorld()->exitAllCitizens(getX(), getY());
    if(getWorld()->isOverlap(getX(), getY(), getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY()))
        if(getWorld()->getNCitizens() == 0)
            {
                getWorld()->finishCurrentLevel();
            }
    return;
}

Pit::Pit(StudentWorld* SWp,double startX, double startY)
:Actor(SWp,IID_PIT,startX,startY,right,0)
{}

void Pit::doSomething()
{
    getWorld()->trapAlltrapableActors(getX(), getY());
    return;
}

LivingThings::LivingThings(StudentWorld* SWp, int imageID, double startX, double startY, Direction dir,int depth)
:Actor(SWp,imageID,startX,startY,dir,depth),m_alive(true)
{}

LivingThings::~LivingThings()
{}

void LivingThings::doSomething()
{
    // Do the common things that LivingThings do
    if(!isAlive())
        return;
    // Do the different things that different LivingThings do
    doDiffLivingThingsDo();
}


Projectile::Projectile(StudentWorld* SWp, int imageID, double startX, double startY, Direction dir)
:LivingThings(SWp,imageID,startX,startY,dir,0),m_timeRemaining(2)
{}

Projectile::~Projectile()
{}

void Projectile::doDiffLivingThingsDo()
{
    if(m_timeRemaining == 0)
    {
        goDie();
        return;
    }
    m_timeRemaining--;
    doDiffProjectileDo();
}

Flame::Flame(StudentWorld* SWp,double startX, double startY,Direction dir)
:Projectile(SWp,IID_FLAME,startX,startY,dir)
{}

void Flame::doDiffProjectileDo()
{
    getWorld()->fireAllFireableActors(getX(), getY());
}

Vomit::Vomit(StudentWorld* SWp,double startX, double startY,Direction dir)
:Projectile(SWp,IID_VOMIT,startX,startY,dir)
{}

void Vomit::doDiffProjectileDo()
{
    if(getWorld()->isOverlap(getX(), getY(), getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY()))
        getWorld()->getPenelope()->getInfected();
    getWorld()->infectAllInfectableCitizens(getX(), getY());
    return;
}


Goodie::Goodie(StudentWorld* SWp, int imageID, double startX, double startY)
:LivingThings(SWp,imageID,startX,startY,right,1)
{}

Goodie::~Goodie()
{}

void Goodie::doDiffLivingThingsDo()
{
    
    if(getWorld()->isOverlap(getX(), getY(), getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY()))
    {
        getWorld()->increaseScore(50);
        goDie();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        // Different behaviors for different goodies
        doDiffGoodiesDo();
    }
}

VaccineGoodie::VaccineGoodie(StudentWorld* SWp,double startX, double startY)
:Goodie(SWp,IID_VACCINE_GOODIE,startX,startY)
{}

void VaccineGoodie::doDiffGoodiesDo()
{
    getWorld()->pickUpVaccineGoodie();
}

GasCanGoodie::GasCanGoodie(StudentWorld* SWp,double startX, double startY)
:Goodie(SWp,IID_GAS_CAN_GOODIE,startX,startY)
{}

void GasCanGoodie::doDiffGoodiesDo()
{
    getWorld()->pickUpGasCanGoodie();
}

LandmineGoodie::LandmineGoodie(StudentWorld* SWp,double startX, double startY)
:Goodie(SWp,IID_LANDMINE_GOODIE,startX,startY)
{}

void LandmineGoodie::doDiffGoodiesDo()
{
    getWorld()->pickUpLandmineGoodie();
}

Zombie::Zombie(StudentWorld* SWp,double startX, double startY)
:LivingThings(SWp,IID_ZOMBIE,startX,startY,right,0),m_paralyzed(true),m_movePlanDistance(0)
{}


Zombie::~Zombie()
{}

void Zombie::doDiffLivingThingsDo()
{
    // Do common things that both types of zombies do
    // #2
    if(m_paralyzed)
    {
        m_paralyzed = false;
        return;
    }
    // #3
    else
    {
        m_paralyzed = true;
        getWorld()->vomitAPerson(getX(),getY(),getDirection());
    }
    // #4
    if(m_movePlanDistance == 0)
    {
        m_movePlanDistance = randInt(3, 10);
        int randDir = randInt(1, 4);
        switch (randDir) {
            case 1:
                setDirection(up);
                break;
            case 2:
                setDirection(down);
                break;
            case 3:
                setDirection(left);
                break;
            case 4:
                setDirection(right);
                break;
            default:
                break;
        }
    }
    
    // Do different things that dumb and smart zombies do
    doDiffZombiesDo();
    // Do common things that both types of zombies do
    // #5 
    int dest_x = getX();
    int dest_y = getY();
    switch (getDirection())
    {
        case up:
            dest_y += 1;
            break;
        case down:
            dest_y -= 1;
            break;
        case left:
            dest_x -= 1;
            break;
        case right:
            dest_x += 1;
            break;
        default:
            break;
    }
    // #6
    if(getWorld()->canBeThere(dest_x, dest_y))
    {
        moveTo(dest_x, dest_y);
        m_movePlanDistance--;
    }
    // #7
    else
    {
        m_movePlanDistance = 0;
    }

}

DumbZombie::DumbZombie(StudentWorld* SWp,double startX, double startY)
:Zombie(SWp,startX,startY)
{}

void DumbZombie::doDiffZombiesDo()
{}

SmartZombie::SmartZombie(StudentWorld* SWp,double startX, double startY)
:Zombie(SWp,startX,startY)
{}

void SmartZombie::doDiffZombiesDo()
{

    // 4b
    Actor* closestPerson = getWorld()->getClosestPerson(getX(), getY());
    
    int dmin2 = getWorld()->distSquare(getX(), getY(), closestPerson->getX(), closestPerson->getY());
    // 4c i
    // if d > 80, then behaves like a dumb zombie. This common behavior has been factored out
    
    // 4c ii
    if(dmin2 <= 80*80)
    {
        if(closestPerson->getX() > getX() && closestPerson->getY() > getY())
        {
            // Choose between right and up
            int randDir = randInt(1, 2);
            if (randDir ==1)
                setDirection(right);
            else
                setDirection(up);
        }
        else if(closestPerson->getX() < getX() && closestPerson->getY() > getY())
        {
            // Choose between left and up
            int randDir = randInt(1, 2);
            if (randDir == 1)
                setDirection(left);
            else
                setDirection(up);
        }
        else if(closestPerson->getX() < getX() && closestPerson->getY() < getY())
        {
            // Choose between left and down
            int randDir = randInt(1, 2);
            if (randDir == 1)
                setDirection(left);
            else
                setDirection(down);
        }
        else if(closestPerson->getX() > getX() && closestPerson->getY() < getY())
        {
            // Choose between right and down
            int randDir = randInt(1, 2);
            if (randDir == 1)
                setDirection(right);
            else
                setDirection(down);
        }
        else if(closestPerson->getX() > getX() && closestPerson->getY() == getY())
            setDirection(right);
        else if(closestPerson->getX() < getX() && closestPerson->getY() == getY())
            setDirection(left);
        else if(closestPerson->getX() == getX() && closestPerson->getY() > getY())
            setDirection(up);
        else if(closestPerson->getX() == getX() && closestPerson->getY() < getY())
            setDirection(down);
    }
    
    // 5
    return;
}

Citizen::Citizen(StudentWorld* SWp,double startX, double startY)
:LivingThings(SWp,IID_CITIZEN,startX,startY,right,0),m_infectionStatus(false),m_infectionCount(0),m_paralyzed(true)
{}

void Citizen::doDiffLivingThingsDo()
{
    // #2
    if(m_infectionStatus)
        m_infectionCount++;
    if(m_infectionCount >= 500)
    {
        goDie();
        getWorld()->decNCitizens();
        getWorld()->playSound(SOUND_ZOMBIE_BORN);
        getWorld()->increaseScore(-1000);
        int temp = randInt(1,10);
        if(temp <= 7)
            getWorld()->addActor(new DumbZombie(getWorld(),getX(),getY()));
        else
            getWorld()->addActor(new SmartZombie(getWorld(),getX(),getY()));
        return;
    }
    // #3
    else
        if(m_paralyzed)
        {
            m_paralyzed = false;
            return;
        }
        else
        {
            m_paralyzed = true;
            int xPenelope = getWorld()->getPenelope()->getX();
            int yPenelope = getWorld()->getPenelope()->getY();
            // #4
            int dist_p2 = getWorld()->distSquare(getX(), getY(), xPenelope, yPenelope);
            // #5
            Actor* closestZombie = getWorld()->getClosestZombie(getX(), getY());
            int dist_z2 = VIEW_WIDTH * VIEW_WIDTH + VIEW_HEIGHT * VIEW_HEIGHT;
            if(closestZombie != nullptr)
                dist_z2 = getWorld()->distSquare(getX(), getY(), closestZombie->getX(), closestZombie->getY());
            // #6
            if(dist_p2 < dist_z2 && dist_p2 <= 80 * 80)
            {
                // a
                if( xPenelope > getX() && yPenelope == getY())
                {
                    if(getWorld()->canBeThere(getX()+2, getY()))
                    {
                        setDirection(right);
                        moveTo(getX()+2, getY());
                        return;
                    }
                }
                else if ( xPenelope == getX() && yPenelope > getY())
                {
                    if(getWorld()->canBeThere(getX(), getY()+2))
                    {
                        setDirection(up);
                        moveTo(getX(), getY()+2);
                        return;
                    }
                }
                else if ( xPenelope < getX() && yPenelope == getY())
                {
                    if(getWorld()->canBeThere(getX()-2, getY()))
                    {
                        setDirection(left);
                        moveTo(getX()-2, getY());
                        return;
                    }
                }
                else if ( xPenelope == getX() && yPenelope < getY())
                {
                    if(getWorld()->canBeThere(getX(), getY()-2))
                    {
                        setDirection(down);
                        moveTo(getX(), getY()-2);
                        return;
                    }
                }
                // b
                else if( xPenelope > getX() && yPenelope > getY())
                {
                    // Choose between right and up
                    int randDir = randInt(1, 2);
                    if (randDir == 1)
                    {
                        // See if can move right first. If not, try moving up
                        if(getWorld()->canBeThere(getX()+2, getY()))
                        {
                            setDirection(right);
                            moveTo(getX()+2, getY());
                            return;
                        }
                        // Cannot move right. So try and see if it can move up
                        else if(getWorld()->canBeThere(getX(), getY()+2))
                        {
                            setDirection(up);
                            moveTo(getX(), getY()+2);
                            return;
                        }
                    }
                    else
                    {
                        // See if can move up first. If not, try moving right
                        if(getWorld()->canBeThere(getX(), getY()+2))
                        {
                            setDirection(up);
                            moveTo(getX(), getY()+2);
                            return;
                        }
                        // Cannot move up. So try and see if it can move right
                        else if(getWorld()->canBeThere(getX()+2, getY()))
                        {
                            setDirection(right);
                            moveTo(getX()+2, getY());
                            return;
                        }
                        
                    }
                }
                else if( xPenelope < getX() && yPenelope > getY())
                {
                    // Choose between left and up
                    int randDir = randInt(1, 2);
                    if (randDir == 1)
                    {
                        // See if can move left first. If not, try moving up
                        if(getWorld()->canBeThere(getX()-2, getY()))
                        {
                            setDirection(left);
                            moveTo(getX()-2, getY());
                            return;
                        }
                        // Cannot move left. So try and see if it can move up
                        else if(getWorld()->canBeThere(getX(), getY()+2))
                        {
                            setDirection(up);
                            moveTo(getX(), getY()+2);
                            return;
                        }
                    }
                    else
                    {
                        // See if can move up first. If not, try moving left
                        if(getWorld()->canBeThere(getX(), getY()+2))
                        {
                            setDirection(up);
                            moveTo(getX(), getY()+2);
                            return;
                        }
                        // Cannot move up. So try and see if it can move left
                        else if(getWorld()->canBeThere(getX()-2, getY()))
                        {
                            setDirection(left);
                            moveTo(getX()-2, getY());
                            return;
                        }
                        
                    }
                }
                else if( xPenelope < getX() && yPenelope < getY())
                {
                    // Choose between left and down
                    int randDir = randInt(1, 2);
                    if (randDir == 1)
                    {
                        // See if can move left first. If not, try moving down
                        if(getWorld()->canBeThere(getX()-2, getY()))
                        {
                            setDirection(left);
                            moveTo(getX()-2, getY());
                            return;
                        }
                        // Cannot move left. So try and see if it can move down
                        else if(getWorld()->canBeThere(getX(), getY()-2))
                        {
                            setDirection(down);
                            moveTo(getX(), getY()-2);
                            return;
                        }
                    }
                    else
                    {
                        // See if can move down first. If not, try moving left
                        if(getWorld()->canBeThere(getX(), getY()-2))
                        {
                            setDirection(down);
                            moveTo(getX(), getY()-2);
                            return;
                        }
                        // Cannot move down. So try and see if it can move left
                        else if(getWorld()->canBeThere(getX()-2, getY()))
                        {
                            setDirection(left);
                            moveTo(getX()-2, getY());
                            return;
                        }
                        
                    }
                    
                }
                else if( xPenelope > getX() && yPenelope < getY())
                {
                    // Choose between right and down
                    int randDir = randInt(1, 2);
                    if (randDir == 1)
                    {
                        // See if can move right first. If not, try moving down
                        if(getWorld()->canBeThere(getX()+2, getY()))
                        {
                            setDirection(right);
                            moveTo(getX()+2, getY());
                            return;
                        }
                        // Cannot move right. So try and see if it can move down
                        else if(getWorld()->canBeThere(getX(), getY()-2))
                        {
                            setDirection(down);
                            moveTo(getX(), getY()-2);
                            return;
                        }
                    }
                    else
                    {
                        // See if can move down first. If not, try moving right
                        if(getWorld()->canBeThere(getX(), getY()-2))
                        {
                            setDirection(down);
                            moveTo(getX(), getY()-2);
                            return;
                        }
                        // Cannot move down. So try and see if it can move right
                        else if(getWorld()->canBeThere(getX()+2, getY()))
                        {
                            setDirection(right);
                            moveTo(getX()+2, getY());
                            return;
                        }
                        
                    }
                    
                }
            }
            // # 7
            else if(dist_z2 <= 80 * 80) // At least there is 1 zombie
            {
                Direction bestDir = getDirection();
                int furthestDist2 = dist_z2;
                bool willMove = false;
                // If the citizen can move up
                if(getWorld()->canBeThere(getX(), getY()+2))
                {
                    Actor* newClosestZombie = getWorld()->getClosestZombie(getX(), getY()+2);
                    int newDist_z2 = getWorld()->distSquare(getX(), getY()+2, newClosestZombie->getX(), newClosestZombie->getY());
                    if(newDist_z2 > furthestDist2)
                    {
                        willMove = true;
                        bestDir = up;
                        furthestDist2 = newDist_z2;
                    }
                    
                }
                // If the citizen can move down
                if(getWorld()->canBeThere(getX(), getY()-2))
                {
                    Actor* newClosestZombie = getWorld()->getClosestZombie(getX(), getY()-2);
                    int newDist_z2 = getWorld()->distSquare(getX(), getY()-2, newClosestZombie->getX(), newClosestZombie->getY());
                    if(newDist_z2 > furthestDist2)
                    {
                        willMove = true;
                        bestDir = down;
                        furthestDist2 = newDist_z2;
                    }
                    
                }
                // If the citizen can move left
                if(getWorld()->canBeThere(getX()-2, getY()))
                {
                    Actor* newClosestZombie = getWorld()->getClosestZombie(getX()-2, getY());
                    int newDist_z2 = getWorld()->distSquare(getX()-2, getY(), newClosestZombie->getX(), newClosestZombie->getY());
                    if(newDist_z2 > furthestDist2)
                    {
                        willMove = true;
                        bestDir = left;
                        furthestDist2 = newDist_z2;
                    }
                }
                // If the citizen can move right
                if(getWorld()->canBeThere(getX()+2, getY()))
                {
                    Actor* newClosestZombie = getWorld()->getClosestZombie(getX()+2, getY());
                    int newDist_z2 = getWorld()->distSquare(getX()+2, getY(), newClosestZombie->getX(), newClosestZombie->getY());
                    if(newDist_z2 > furthestDist2)
                    {
                        willMove = true;
                        bestDir = right;
                        furthestDist2 = newDist_z2;
                    }
                }
                //
                if(!willMove)
                    return;
                else
                {
                    setDirection(bestDir);
                    switch (bestDir)
                    {
                        case up:
                            moveTo(getX(), getY()+2);
                            break;
                        case down:
                            moveTo(getX(), getY()-2);
                            break;
                        case left:
                            moveTo(getX()-2, getY());
                            break;
                        case right:
                            moveTo(getX()+2, getY());
                            break;
                        default:
                            break;
                    }
                    return;
                }
            
            }
        }
    
        
}

Landmine::Landmine(StudentWorld* SWp,double startX, double startY)
:LivingThings(SWp,IID_LANDMINE,startX,startY,right,1),m_timeLeft(30),m_isActive(false)
{}

void Landmine::doDiffLivingThingsDo()
{
    if(getWorld()->flameTriggerLandmine(getX(), getY()))
        explode();
    
    if(!m_isActive)
    {
        m_timeLeft--;
        if(m_timeLeft == 0)
            m_isActive = true;
    }
    else
    {
        if(getWorld()->anythingStepOnLandmine(getX(), getY()))
        {
            explode();
        }
    }
        
}

void Landmine::explode()
{
    goDie();
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
    getWorld()->addActor(new Pit(getWorld(),getX(),getY()));
    for(int i = -1; i <= 1; i++)
        for(int j = -1; j <= 1; j++)
            if(getWorld()->canFireThere(getX()+i*SPRITE_WIDTH,getY()+j*SPRITE_HEIGHT))
                getWorld()->addActor(new Flame(getWorld(),getX()+i*SPRITE_WIDTH,getY()+j*SPRITE_HEIGHT,up));
}
