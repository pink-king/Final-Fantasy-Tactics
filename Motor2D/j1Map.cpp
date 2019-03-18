#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"

#include "Brofiler/Brofiler.h"

#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.assign("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.assign(config.child("folder").child_value());
	return ret;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("MAP DRAW", Profiler::Color::DeepPink);

	if (map_loaded == false)
		return;

	int mapBlitOffset = 50;//TODO: Get this from the xml

	std::list<MapLayer*>::iterator layer = data.layers.begin();
	for (; layer != data.layers.end(); ++layer)
	{
		if ((*layer)->name == "navigationLayer" && showNavLayer == false) {
			continue;
		}

		for (int i = 0; i < data.height; ++i)
		{
			for (int j = 0; j < data.width; ++j)
			{
				if (App->render->IsOnCamera(MapToWorld(i, j).x, MapToWorld(i, j).y, data.tile_width, data.tile_height))
				{
					int tile_id = (*layer)->Get(i, j);
					if (tile_id > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);
						if (tileset != nullptr)
						{
							SDL_Rect r = tileset->GetTileRect(tile_id);
							iPoint pos = MapToWorld(i, j);

							App->render->Blit(tileset->texture, pos.x, pos.y, &r, (*layer)->properties.parallaxSpeed);

						}
					}
				}
			}
		}
	}
}

int Properties::Get(const char* value, int default_value) const
{
	std::list<Property*>::const_iterator item;
	item = list.begin();

	while(item != list.end())
	{
		if((*item)->name == value)
			return (*item)->value;
		++item;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>:: const_iterator item = data.tilesets.begin();
	TileSet* set = *item;

	while(item != data.tilesets.end())
	{
		if(id < (*item)->firstgid)
		{
			set = *(--item);
			++item;
			break;
		}
		set = *item;
		++item;
	}

	return set;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2) - 1;
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	if (num_tiles_width != 0)
	{
		
		rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
		rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	}
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	std::list<TileSet*>::iterator tiles_item;
	tiles_item = data.tilesets.begin();

	while(tiles_item != data.tilesets.end())
	{
		RELEASE(*tiles_item);
		++tiles_item;
	}
	data.tilesets.clear();

	// Remove all layers
	std::list<MapLayer*>::iterator layer_item;
	layer_item = data.layers.begin();

	while(layer_item != data.layers.end())
	{
		RELEASE(*layer_item);
		++layer_item;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	pugi::xml_parse_result result = map_file.load_file(file_name);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		// stores the current level string name
		data.loadedLevel.assign(file_name);
		// ------------------------------------
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.push_back(lay);
	}

	// Load objects/scene colliders -----------------------------------------
	pugi::xml_node objectGroup;
	for (objectGroup = map_file.child("map").child("group"); objectGroup && ret; objectGroup = objectGroup.next_sibling("group"))
	{
		std::string tmp(objectGroup.attribute("name").as_string());
		//MapObjects* obj = new MapObjects();

		if (tmp == "Colliders")
		{
			//for(pugi::xml_node collidersGroup = objectGroup.child("objectgroup"))
			ret = LoadMapColliders(objectGroup);//, obj);
			LOG("loading Map colliders");
		}
		else if (tmp == "Player")
		{
			// TODO, check latest handout
			pugi::xml_node player = objectGroup.child("objectgroup").child("object");
			playerData.name = player.attribute("name").as_string();
			playerData.x = player.attribute("x").as_int();
			playerData.y = player.attribute("y").as_int();

			// load custom properties
			LoadProperties(objectGroup.child("objectgroup"), playerData.properties);
		}
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		std::list<TileSet*>::iterator item = data.tilesets.begin();
		while (item != data.tilesets.end())
		{
			TileSet* s = (*item);
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			++item;
		}

		std::list<MapLayer*>::iterator item_layer = data.layers.begin();
		while (item_layer != data.layers.end())
		{
			MapLayer* l = (*item_layer);
			LOG("Layer ----");
			LOG("name: %s", l->name.data());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			++item_layer;
		}
	}

	map_loaded = ret;

	return ret;
}


// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.length() > 0)
		{
			std::string red, green, blue;
			red = (1, 2);
			green = (3, 4);
			blue = (5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
		// load custom properties from map data
		pugi::xml_node propertiesNode = map.child("properties");

		if (propertiesNode == NULL)
		{
			LOG("Map without custom properties");
		}
		else
		{
			LoadProperties(map, data.properties);
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.assign(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();

	pugi::xml_node offset = tileset_node.child("tileoffset");
	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.data(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

bool j1Map::LoadMapColliders(pugi::xml_node& node)//, MapObjects* obj)
{
	bool ret = true;

	SDL_Rect colliderRect;

	// iterate all objectgroups
	for (pugi::xml_node objectGroup = node.child("objectgroup"); objectGroup && ret; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		std::string tmp = objectGroup.attribute("name").as_string();
		MapObjects* newObject = new MapObjects();

		newObject->name = tmp.data();

		// Load custom properties
		LoadProperties(objectGroup, newObject->properties);

		//bool counted = false;

		// iterate all objects
		int i = 0; // to allocate i colliders at once on new MapObject pointer for map reference?, not implemented yet

		for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
		{
			// count the num of objects
			/*if (!counted)
			{
			for (pugi::xml_node objectCounter = objectGroup.child("object"); object; object = object.next_sibling("object"))
			{
			++i;
			}
			counted = true;
			}*/

			colliderRect.x = object.attribute("x").as_int(0);
			colliderRect.y = object.attribute("y").as_int(0);
			colliderRect.h = object.attribute("height").as_int(0);
			colliderRect.w = object.attribute("width").as_int(0);
			// create collider type of
			// increments counter
			++i;
		}

		// add object to list
		data.mapObjects.push_back(*newObject);
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node propertiesNode = node.child("properties");

	if (propertiesNode == NULL)
	{
		LOG("properties not found");
		ret = false;
	}
	else
	{
		properties.draw = propertiesNode.find_child_by_attribute("name", "Draw").attribute("value").as_bool(true);
		properties.navigation = propertiesNode.find_child_by_attribute("name", "Navigation").attribute("value").as_bool(false);
		properties.testValue = propertiesNode.find_child_by_attribute("name", "testValue").attribute("value").as_int(0);
		properties.parallaxSpeed = propertiesNode.find_child_by_attribute("name", "parallaxSpeed").attribute("value").as_float(1.0F);
	//	properties.music_name = propertiesNode.find_child_by_attribute("name", "background_music").attribute("value").as_string();
		//properties.fx_name = propertiesNode.find_child_by_attribute("name", "background_music").attribute("value").as_string();

		// associated gui xml with this map ----
	//	properties.gui_xml_path = propertiesNode.find_child_by_attribute("name", "associated_gui_xml").attribute("value").as_string();
	}

	return ret;
}


bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	std::list<MapLayer*>:: const_iterator item;
	item = data.layers.begin();

	for(; *item != NULL; ++item)
	{
		MapLayer* layer = *item;

		if(layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;
				
				if(tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}