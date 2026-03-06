#pragma once

#include "render_data.h"

struct Tile
{
	Tile() :
		id(0),
		renderData()
	{
	}

	Tile(int _id, edupt::RenderData _renderData) :
		id(_id),
		renderData(_renderData)
	{
	}

	uint32_t id;
	edupt::RenderData renderData;
};
