/* Copyright (C) 2013-2014 Michal Brzozowski (rusolis@poczta.fm)

   This file is part of KeeperRL.

   KeeperRL is free software; you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   KeeperRL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.
   If not, see http://www.gnu.org/licenses/ . */

#ifndef _LEVEL_H
#define _LEVEL_H

#include "util.h"
#include "debug.h"
#include "field_of_view.h"
#include "square_factory.h"
#include "vision.h"
#include "unique_entity.h"
#include "bucket_map.h"
#include "player_message.h"
#include "movement_type.h"
#include "sectors.h"

class Model;
class Square;
class Player;
class LevelMaker;
class Location;
class Attack;
class CollectiveBuilder;
class ProgressMeter;
class Sectors;
class Tribe;

RICH_ENUM(SquareAttrib,
  NO_DIG,
  GLACIER,
  MOUNTAIN,
  HILL,
  LOWLAND,
  CONNECT_ROAD, 
  CONNECT_CORRIDOR,
  LAKE,
  RIVER,
  ROAD_CUT_THRU,
  NO_ROAD,
  ROOM,
  COLLECTIVE_START,
  COLLECTIVE_STAIRS,
  EMPTY_ROOM,
  FOG,
  FORREST
);


/** A class representing a single level of the dungeon or the overworld. All events occuring on the level are performed by this class.*/
class Level : public UniqueEntity<Level> {
  public:

  ~Level();
  static Rectangle getMaxBounds();
  static Rectangle getSplashBounds();
  static Rectangle getSplashVisibleBounds();

  /** Checks if the creature can move to \paramname{direction}. This ensures 
    * that a subsequent call to #moveCreature will not fail.*/
  bool canMoveCreature(const Creature*, Vec2 direction) const;

  /** Moves the creature. Updates the creature's position.*/
  void moveCreature(Creature*, Vec2 direction);

  /** Swaps positions of two creatures. */
  void swapCreatures(Creature*, Creature*);

  /** Puts \paramname{creature} on \paramname{position}. \paramname{creature} ownership is assumed by the model.*/
  void addCreature(Vec2 position, PCreature creature);

  /** Puts the \paramname{creature} on \paramname{position}. */
  void putCreature(Vec2 position, Creature* creature);

  //@{
  /** Finds an appropriate square for the \paramname{creature} changing level from \paramname{direction}.
    The square's method Square::isLandingSquare must return true for \paramname{direction}. 
    Returns the position of the stairs that were used. */
  Vec2 landCreature(StairDirection direction, StairKey key, Creature* creature);
  Vec2 landCreature(StairDirection direction, StairKey key, PCreature creature);
  //@}

  /** Lands the creature on the level randomly choosing one of the given squares.
      Returns the position of the stairs that were used.*/
  Vec2 landCreature(vector<Vec2> landing, PCreature creature);
  Vec2 landCreature(vector<Vec2> landing, Creature* creature);

  /** Returns the landing squares for given direction and stair key. See Square::getLandingLink() */
  vector<Vec2> getLandingSquares(StairDirection, StairKey) const;

  /** Removes the creature from \paramname{position} from the level and model. The creature object is retained.*/
  void killCreature(Creature*);

  /** Recalculates visibility data assuming that \paramname{changedSquare} has changed
      its obstructing/non-obstructing attribute. */
  void updateVisibility(Vec2 changedSquare);

  /** Returns width of the level.*/
  int getWidth() const;

  /** Returns height of the level.*/
  int getHeight() const;

  /** Checks \paramname{pos} lies within the level's boundaries.*/
  bool inBounds(Vec2 pos) const;

  /** Returns the level's boundaries.*/
  Rectangle getBounds() const;

  /** Returns the name of the level. */
  const string& getName() const;

  //@{
  /** Returns the given square. \paramname{pos} must lie within the boundaries. */
  vector<const Square*> getSquare(Vec2) const;
  vector<Square*> getSquare(Vec2);
  vector<const Square*> getSquares(const vector<Vec2>& pos) const;
  vector<Square*> getSquares(const vector<Vec2>&);
  const Square* getSafeSquare(Vec2) const;
  Square* getSafeSquare(Vec2);
  //@}

  void replaceSquare(Vec2 pos, PSquare square);

  /** The given square's method Square::tick() will be called every turn. */
  void addTickingSquare(Vec2 pos);

  /** Ticks all squares that must be ticked. */
  void tick(double time);

  /** Moves the creature to a different level according to \paramname{direction}. */
  void changeLevel(StairDirection direction, StairKey key, Creature* c);

  /** Moves the creature to a given level. */
  void changeLevel(Level* destination, Vec2 landing, Creature* c);

  /** Performs a throw of the item, with all consequences of the event.*/
  void throwItem(PItem item, const Attack& attack, int maxDist, Vec2 position, Vec2 direction, VisionId);
  void throwItem(vector<PItem> item, const Attack& attack, int maxDist, Vec2 position, Vec2 direction, VisionId);

  /** Sets the creature that is assumed to be the player.*/
  void updatePlayer();

  /** Sets the level to be rendered in the background with given offset.*/
  void setBackgroundLevel(const Level*, Vec2 offset);

  //@{
  /** Returns all creatures on this level. */
  const vector<Creature*>& getAllCreatures() const;
  vector<Creature*>& getAllCreatures();
  vector<Creature*> getAllCreatures(Rectangle bounds) const;
  //@}

  /** Checks whether the creature can see the square.*/
  bool canSee(const Creature* c, Vec2 to) const;

  /** Returns if it's possible to see the given square.*/
  bool canSee(Vec2 from, Vec2 to, VisionId) const;

  /** Returns all tiles visible by a creature.*/
  vector<Vec2> getVisibleTiles(const Creature*) const;
  vector<Vec2> getVisibleTiles(Vec2 pos, VisionId) const;

  /** Checks if the player can see a given square.*/
  bool playerCanSee(Vec2 pos) const;

  /** Checks if the player can see a given creature.*/
  bool playerCanSee(const Creature*) const;

  /** Displays \paramname{playerCanSee} message if the player can see position \paramname{pos},
    and \paramname{cannot} otherwise.*/
  void globalMessage(Vec2 position, const PlayerMessage& playerCanSee, const PlayerMessage& cannot = "") const;

  /** Displays \paramname{playerCanSee} message if the player can see the creature, 
    and \paramname{cannot} otherwise.*/
  void globalMessage(const Creature*, const PlayerMessage& ifPlayerCanSee, const PlayerMessage& cannot) const;

  /** Returns the player creature.*/
  const Creature* getPlayer() const;

  /** Returns name of the given location. Returns nullptr if none. */
  const Location* getLocation(Vec2) const;

  const vector<Location*> getAllLocations() const;

  struct CoverInfo : public NamedTupleBase<bool, double> {
    NAMED_TUPLE_STUFF(CoverInfo);
    NAME_ELEM(0, covered);
    NAME_ELEM(1, sunlight);
  };

  CoverInfo getCoverInfo(Vec2) const;

  const Model* getModel() const;
  Model* getModel();

  void addLightSource(Vec2, double radius);
  void removeLightSource(Vec2, double radius);

  /** Returns the amount of light in the square, capped within (0, 1).*/
  double getLight(Vec2) const;

  /** Returns the amount of sunlight in the square, capped within (0, 1).*/
  double isInSunlight(Vec2 pos) const;

  /** Returns if two squares are connected assuming given movement.*/
  bool areConnected(Vec2, Vec2, const MovementType&) const;

  void updateConnectivity(Vec2);
  void updateSunlightMovement();

  /* Notify that there are squares enterable only by this tribe and custom movement type needs to be tracked.*/
  void addSquareOwner(const Tribe*);

  /** Class used to initialize a level object.*/
  class Builder {
    public:
    /** Constructs a builder with given size and name. */
    Builder(ProgressMeter&, int width, int height, const string& name, bool covered = true);
    
    /** Move constructor.*/
    Builder(Builder&&) = default;

    /** Returns a given square.*/
    Square* getSquare(Vec2);

    /** Checks if it's possible to put a creature on given square.*/
    bool canPutCreature(Vec2, Creature*);

    /** Puts a creatue on a given square. If the square is later changed to something else, the creature remains.*/
    void putCreature(Vec2, PCreature);

    /** Puts items on a given square. If the square is later changed to something else, the items remain.*/
    void putItems(Vec2, vector<PItem> items);

    /** Sets the message displayed when the player first enters the level.*/
    void setMessage(const string& message);

    /** Builds the level. The level will keep reference to the model.
        \paramname{surface} tells if this level is on the Earth surface.*/
    PLevel build(Model*, LevelMaker*);

    //@{
    /** Puts a square on given position. Sets optional attributes of the square. The attributes remain if the square is changed.*/
    void putSquare(Vec2, SquareType, optional<SquareAttrib> = none);
    void putSquare(Vec2, SquareType, vector<SquareAttrib> attribs);
    void putSquare(Vec2, PSquare, SquareType, optional<SquareAttrib> = none);
    void putSquare(Vec2, PSquare, SquareType, vector<SquareAttrib> attribs);
    //@}

    /** Returns the square type.*/
    const SquareType& getType(Vec2);

    /** Checks if the given square has an attribute.*/
    bool hasAttrib(Vec2 pos, SquareAttrib attr);

    /** Adds attribute to given square. The attribute will remain if the square is changed.*/
    void addAttrib(Vec2 pos, SquareAttrib attr);

    /** Removes attribute from given square.*/
    void removeAttrib(Vec2 pos, SquareAttrib attr);

    /** Sets the height of the given square.*/
    void setHeightMap(Vec2 pos, double h);

    /** Returns the height of the given square.*/
    double getHeightMap(Vec2 pos);

    /** Adds a location to the level and sets its coordinates.*/
    void addLocation(Location*, Rectangle area);

    /** Adds a collective to the level and initializes it.*/
    void addCollective(CollectiveBuilder*);

    /** Sets the cover of the square. The value will remain if square is changed.*/
    void setCoverInfo(Vec2, CoverInfo);
   
    enum Rot { CW0, CW1, CW2, CW3};

    void pushMap(Rectangle bounds, Rot);
    void popMap();
    
    private:
    bool isInSunlight(Vec2);
    Vec2 transform(Vec2);
    Table<PSquare> squares;
    Table<double> heightMap;
    Table<double> dark;
    vector<Location*> locations;
    vector<CollectiveBuilder*> collectives;
    Table<CoverInfo> coverInfo;
    Table<EnumSet<SquareAttrib>> attrib;
    Table<SquareType> type;
    vector<PCreature> creatures;
    Table<vector<PItem>> items;
    string entryMessage;
    string name;
    vector<Vec2::LinearMap> mapStack;
    ProgressMeter& progressMeter;
  };

  typedef unique_ptr<Builder> PBuilder;

  SERIALIZATION_DECL(Level);

  private:
  Vec2 transform(Vec2);
  Table<PSquare> SERIAL(squares);
  map<pair<StairDirection, StairKey>, vector<Vec2>> SERIAL(landingSquares);
  vector<Location*> SERIAL(locations);
  set<Vec2> SERIAL(tickingSquares);
  vector<Creature*> SERIAL(creatures);
  Model* SERIAL2(model, nullptr);
  mutable EnumMap<VisionId, FieldOfView> SERIAL(fieldOfView);
  string SERIAL(entryMessage);
  string SERIAL(name);
  Creature* SERIAL2(player, nullptr);
  const Level* SERIAL2(backgroundLevel, nullptr);
  Vec2 SERIAL(backgroundOffset);
  Table<CoverInfo> SERIAL(coverInfo);
  BucketMap<Creature*> SERIAL(bucketMap);
  Table<double> SERIAL(lightAmount);
  mutable unordered_map<MovementType, Sectors> SERIAL(sectors);
  unordered_set<const Tribe*> SERIAL(squareOwners);
  
  Level(Table<PSquare> s, Model*, vector<Location*>, const string& message, const string& name,
      Table<CoverInfo> coverInfo);

  void addLightSource(Vec2 pos, double radius, int numLight);
  FieldOfView& getFieldOfView(VisionId vision) const;
  vector<Vec2> getVisibleTilesNoDarkness(Vec2 pos, VisionId vision) const;
  bool isWithinVision(Vec2 from, Vec2 to, VisionId) const;

  /** Notify relevant locations about creature position. */
  void notifyLocations(Creature*);
};

#endif
