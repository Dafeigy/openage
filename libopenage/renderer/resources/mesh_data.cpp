// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "mesh_data.h"

#include <array>
#include <cstring>
#include <utility>

#include "../../error/error.h"
#include "../../datastructure/constexpr_map.h"


namespace openage::renderer::resources {

static constexpr auto vin_size = datastructure::create_const_map<vertex_input_t, size_t>(
	std::make_pair(vertex_input_t::F32, 4),
	std::make_pair(vertex_input_t::V2F32, 8),
	std::make_pair(vertex_input_t::V3F32, 12),
	std::make_pair(vertex_input_t::M3F32, 36)
);

static constexpr auto vin_count = datastructure::create_const_map<vertex_input_t, size_t>(
	std::make_pair(vertex_input_t::F32, 1),
	std::make_pair(vertex_input_t::V2F32, 2),
	std::make_pair(vertex_input_t::V3F32, 3),
	std::make_pair(vertex_input_t::M3F32, 9)
);

size_t vertex_input_size(vertex_input_t in) {
	return vin_size.get(in);
}

size_t vertex_input_count(vertex_input_t in) {
	return vin_count.get(in);
}

VertexInputInfo::VertexInputInfo(std::vector<vertex_input_t> inputs, vertex_layout_t layout, vertex_primitive_t primitive)
	: inputs(std::move(inputs))
	, layout(layout)
	, primitive(primitive) {}

VertexInputInfo::VertexInputInfo(std::vector<vertex_input_t> inputs, vertex_layout_t layout, vertex_primitive_t primitive, index_t index_type)
	: inputs(std::move(inputs))
	, layout(layout)
	, primitive(primitive)
	, index_type(index_type) {}

void VertexInputInfo::add_shader_input_map(std::unordered_map<size_t, size_t>&& in_map) {
	for (auto mapping : in_map) {
		if (unlikely(mapping.first >= this->inputs.size())) {
			throw Error(MSG(err) << "A shader input mapping is out-of-range, exceeding the available number of attributes.");
		}
	}

	this->shader_input_map = std::move(in_map);
}

size_t VertexInputInfo::vert_size() const {
	size_t size = 0;
	for (auto in : this->inputs) {
		size += vertex_input_size(in);
	}
	return size;
}

const std::vector<vertex_input_t> &VertexInputInfo::get_inputs() const {
	return this->inputs;
}

std::optional<std::unordered_map<size_t, size_t>> const& VertexInputInfo::get_shader_input_map() const {
	return this->shader_input_map;
}

vertex_layout_t VertexInputInfo::get_layout() const {
	return this->layout;
}

vertex_primitive_t VertexInputInfo::get_primitive() const {
	return this->primitive;
}

std::optional<index_t> VertexInputInfo::get_index_type() const {
	return this->index_type;
}

MeshData::MeshData(const util::Path &/*path*/) {
	// TODO implement mesh loaders
	throw "unimplemented lol";
}

MeshData::MeshData(std::vector<uint8_t>&& verts, const VertexInputInfo &info)
	: data(std::move(verts))
	, info(info) {}

MeshData::MeshData(std::vector<uint8_t> &&verts, std::vector<uint8_t> &&ids, const VertexInputInfo &info)
	: data(std::move(verts))
	, ids(std::move(ids))
	, info(info) {}

std::vector<uint8_t> const& MeshData::get_data() const {
	return this->data;
}

std::optional<std::vector<uint8_t>> const &MeshData::get_ids() const {
	return this->ids;
}

VertexInputInfo MeshData::get_info() const {
	return *this->info;
}

/// Vertices of a quadrilateral filling the whole screen.
/// Format: (pos, tex_coords) = (x, y, u, v)
static constexpr const std::array<float, 16> QUAD_DATA = { {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f
	} };

MeshData MeshData::make_quad() {
	auto const data_size = QUAD_DATA.size() * sizeof(decltype(QUAD_DATA)::value_type);
	std::vector<uint8_t> verts(data_size);
	std::memcpy(verts.data(), reinterpret_cast<const uint8_t*>(QUAD_DATA.data()), data_size);

	VertexInputInfo info { { vertex_input_t::V2F32, vertex_input_t::V2F32 }, vertex_layout_t::AOS, vertex_primitive_t::TRIANGLE_STRIP };

	return MeshData(std::move(verts), info);
}

}
