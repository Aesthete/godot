
/*************************************************************************/
/*  remote_transform.cpp                                              */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "remote_transform.h"

void RemoteTransform::_update_cache() {
	cache = 0;
	if (has_node(remote_node)) {
		Node *node = get_node(remote_node);
		if (!node || this == node || node->is_a_parent_of(this) || this->is_a_parent_of(node)) {
			return;
		}

		cache = node->get_instance_ID();
	}
}

void RemoteTransform::_update_remote() {

	if (!is_inside_tree())
		return;

	if (!cache)
		return;

	Object *obj = ObjectDB::get_instance(cache);
	if (!obj)
		return;

	Spatial *n = obj->cast_to<Spatial>();
	if (!n)
		return;

	if (!n->is_inside_tree())
		return;

	//todo make faster
	if (use_global_coordinates) {

		if (update_remote_position && update_remote_rotation && update_remote_scale) {
			n->set_global_transform(get_global_transform());
		} else {
			Transform n_trans = n->get_global_transform();
			Transform our_trans = get_global_transform();

			if (!update_remote_position)
				our_trans.set_origin(n_trans.get_origin());

			n->set_global_transform(our_trans);

			if (!update_remote_rotation)
				n->set_rotation(n_trans.basis.get_rotation());

			if (!update_remote_scale)
				n->set_scale(n_trans.basis.get_scale());
		}

	} else {
		if (update_remote_position && update_remote_rotation && update_remote_scale) {
			n->set_global_transform(get_global_transform());
		} else {
			Transform n_trans = n->get_transform();
			Transform our_trans = get_transform();

			if (!update_remote_position)
				our_trans.set_origin(n_trans.get_origin());

			n->set_transform(our_trans);

			if (!update_remote_rotation)
				n->set_rotation(n_trans.basis.get_rotation());

			if (!update_remote_scale)
				n->set_scale(n_trans.basis.get_scale());
		}
	}
}

void RemoteTransform::_notification(int p_what) {

	switch (p_what) {

		case NOTIFICATION_READY: {

			_update_cache();

		} break;
		case NOTIFICATION_TRANSFORM_CHANGED: {
			if (!is_inside_tree())
				break;

			if (cache) {

				_update_remote();
			}

		} break;
	}
}

void RemoteTransform::set_remote_node(const NodePath &p_remote_node) {

	remote_node = p_remote_node;
	if (is_inside_tree())
		_update_cache();

	update_configuration_warning();
}

NodePath RemoteTransform::get_remote_node() const {

	return remote_node;
}

void RemoteTransform::set_use_global_coordinates(const bool p_enable) {
	use_global_coordinates = p_enable;
}

bool RemoteTransform::get_use_global_coordinates() const {
	return use_global_coordinates;
}

void RemoteTransform::set_update_position(const bool p_update) {
	update_remote_position = p_update;
	_update_remote();
}

bool RemoteTransform::get_update_position() const {
	return update_remote_position;
}

void RemoteTransform::set_update_rotation(const bool p_update) {
	update_remote_rotation = p_update;
	_update_remote();
}

bool RemoteTransform::get_update_rotation() const {
	return update_remote_rotation;
}

void RemoteTransform::set_update_scale(const bool p_update) {
	update_remote_scale = p_update;
	_update_remote();
}

bool RemoteTransform::get_update_scale() const {
	return update_remote_scale;
}

String RemoteTransform::get_configuration_warning() const {

	if (!has_node(remote_node) || !get_node(remote_node) || !get_node(remote_node)->cast_to<Spatial>()) {
		return TTR("Path property must point to a valid Spatial node to work.");
	}

	return String();
}

void RemoteTransform::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_remote_node", "path"), &RemoteTransform::set_remote_node);
	ClassDB::bind_method(D_METHOD("get_remote_node"), &RemoteTransform::get_remote_node);

	ClassDB::bind_method(D_METHOD("set_use_global_coordinates", "use_global_coordinates"), &RemoteTransform::set_use_global_coordinates);
	ClassDB::bind_method(D_METHOD("get_use_global_coordinates"), &RemoteTransform::get_use_global_coordinates);

	ClassDB::bind_method(D_METHOD("set_update_position", "update_remote_position"), &RemoteTransform::set_update_position);
	ClassDB::bind_method(D_METHOD("get_update_position"), &RemoteTransform::get_update_position);
	ClassDB::bind_method(D_METHOD("set_update_rotation", "update_remote_rotation"), &RemoteTransform::set_update_rotation);
	ClassDB::bind_method(D_METHOD("get_update_rotation"), &RemoteTransform::get_update_rotation);
	ClassDB::bind_method(D_METHOD("set_update_scale", "update_remote_scale"), &RemoteTransform::set_update_scale);
	ClassDB::bind_method(D_METHOD("get_update_scale"), &RemoteTransform::get_update_scale);

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "remote_path"), "set_remote_node", "get_remote_node");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_global_coordinates"), "set_use_global_coordinates", "get_use_global_coordinates");

	ADD_GROUP("Update", "update_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "update_position"), "set_update_position", "get_update_position");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "update_rotation"), "set_update_rotation", "get_update_rotation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "update_scale"), "set_update_scale", "get_update_scale");
}

RemoteTransform::RemoteTransform() {

	use_global_coordinates = true;
	update_remote_position = true;
	update_remote_rotation = true;
	update_remote_scale = true;

	cache = 0;
	set_notify_transform(true);
}