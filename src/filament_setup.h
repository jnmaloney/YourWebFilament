#pragma once


void* on_mesh_load(void* data, unsigned int data_size);
void* on_material_load(void* data, unsigned int data_size);
void* on_ktx_load(void* data, unsigned int data_size);

void init_filament();
void stop_filament();
void loop_filament();

// from JS, GL context
void create_fila_engine();