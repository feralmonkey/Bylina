#include "AssetStore.h"
#include <spdlog/spdlog.h>
#include <SDL_image.h>

AssetStore::AssetStore() {
	spdlog::info("Asset store constructor called!");
}

AssetStore::~AssetStore() {
	spdlog::info("Asset store destructor called!");
	Clear();
}

void AssetStore::Clear() {
	for (auto texture: textures) {
		SDL_DestroyTexture(texture.second);
	}
	textures.clear(); // this won't de allocate the textures inside the map. We need to clear them individually above. 

	for (auto font : fonts) {
		TTF_CloseFont(font.second);
	}
	fonts.clear();
	
	
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// add the texture to a map
	textures.emplace(assetId, texture);

	spdlog::info("New texture added to Asset Store with ID: " + assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
	return textures[assetId];
}

void AssetStore::AddFont(const std::string& assetId, const std::string& filePath, int fontSize) {
	fonts.emplace(assetId, TTF_OpenFont(filePath.c_str(), fontSize));
}

TTF_Font* AssetStore::GetFont(const std::string& assetId) {
	return fonts[assetId];
}