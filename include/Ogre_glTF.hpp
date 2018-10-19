#pragma once

#include <memory>
#include <Ogre.h>

#include "Ogre_glTF_DLL.hpp"

namespace Ogre_glTF
{
	//Forward declare main class
	class glTFLoader;

	///struct that contains a mesh and the datablock that should be used with it.
	///This represent what an extracted object from a glTF asset contains.
	///The mash should have a skeletonInstance attached to it the glTF file defined a skin;
	struct MeshAndDataBlock
	{
		///Pointer to the Ogre Mesh
		Ogre::MeshPtr Mesh;

		///Pointer to the HlmsDatablock. This should be a HlmsPbsDatablock
		Ogre::HlmsDatablock* datablock;
	};

	// Struct that contains a pointer to the item along with positions, scale and orientation
	// of the node the holds it
	// This transform information can then be used in Ogre to correctly scale etc the item
	struct ItemAndTransforms
	{
		// Pointer to the item
		Ogre::Item* item;

		Ogre::Vector3 pos;	// Position
		Ogre::Vector3 scale;  // scale
		Ogre::Quaternion rot; // rotation
	};

	///Plugin accessible interface that plugin users can use
	struct glTFLoaderInterface
	{
		///Polymorphic dtor
		virtual ~glTFLoaderInterface() = default;

		///Get you an item from a GLB file loaded inside an Ogre resource group
		/// \param name The name of the resource
		/// \param smgr The scene manager where the Item will be used
		/// \return pointer to a created item in your scene manager using the mesh in the glTF asset
		virtual Ogre::Item* getItemFromResource(const std::string& name, Ogre::SceneManager* smgr) = 0;

		///Get you an item and transforms from a GLB file loaded inside an Ogre resource group
		/// \param name The name of the resource
		/// \param smgr The scene manager where the Item will be used
		/// \return a struct containing the item pointer and transforms
		virtual ItemAndTransforms getItemAndTransformsFromResource(const std::string& name, Ogre::SceneManager* smgr) = 0;

		///Get you an item from a GLB or a GLTF file from the filesystem.
		/// \param name The name of the resource
		/// \param smgr The scene manager where the Item will be used
		/// \return pointer to a created item in your scene manager using the mesh in the glTF asset
		virtual Ogre::Item* getItemFromFileSystem(const std::string& fileName, Ogre::SceneManager* smgr) = 0;

		///Get you a mesh and a material datablock from a GLB in the resource manager
		/// \param name The name of the resource
		/// \return a struct containing pointers to a mesh and a datablock
		virtual MeshAndDataBlock getMeshFromResource(const std::string& name) = 0;

		///Gets you a mesh and a material datablock from GLB or a GLTF file from the filesystem
		/// \param name The name of the resource
		/// \return a struct containing pointers to a mesh and a datablock
		virtual MeshAndDataBlock getMeshFromFileSystem(const std::string& name) = 0;
	};

	///Class that hold the loaded content of a glTF file and that can create Ogre objects from it
	class Ogre_glTF_EXPORT loaderAdapter
	{
		friend class glTFLoader;

		///opaque content of the class
		struct impl;

		///pointer to implementation
		std::unique_ptr<impl> pimpl;

		std::string adapterName;

	public:
		///This will also initialize the "pimpl" structure
		loaderAdapter();

		///This clear the pimpl structure
		~loaderAdapter();

		///Deleted copy constructor : non copyable class
		loaderAdapter(const loaderAdapter&) = delete;

		///Deleted assignment constructor : non copyable class
		loaderAdapter& operator=(const loaderAdapter&) = delete;

		Ogre::MeshPtr getMesh() const;
		Ogre::HlmsDatablock* getDatablock() const;
		void getTransforms(ItemAndTransforms* tran);

		///Construct an item for this object
		/// \param smgr pointer to the scene manager where we are creating the item
		Ogre::Item* getItem(Ogre::SceneManager* smgr) const;

		///Move constructor : object is movable
		/// \param other object to move
		loaderAdapter(loaderAdapter&& other) noexcept;

		///Move assignment operator
		loaderAdapter& operator=(loaderAdapter&& other) noexcept;

		///Return the current state of the adapter
		bool isOk() const;

		///Return the last error generated by the underlying glTF loading library
		std::string getLastError() const;
	};

	///Class that is responsible for initializing the library with the loader, and giving out
	class Ogre_glTF_EXPORT glTFLoader final : public glTFLoaderInterface
	{
		///object that acutally communicate with the underlying glTF loading library
		struct glTFLoaderImpl;

		///Opaque pointer that handle the underlying glTF loading library (pimpl)
		std::unique_ptr<glTFLoaderImpl> loaderImpl;

	public:
		///Initialize the library by creating this object.
		glTFLoader();

		///Move constructor
		/// \param other object to move
		glTFLoader(glTFLoader&& other) noexcept;

		glTFLoader& operator=(glTFLoader&& other) noexcept;

		///Deinitialize the library at this object destruction
		~glTFLoader();

		///Load a glTF text or binary file. Give you an adapter to use this file with Ogre
		/// \param path String containing the path to a file to load (either .glTF or .glc)
		loaderAdapter loadFromFileSystem(const std::string& path) const;

		loaderAdapter loadGlbResource(const std::string& name) const;

		Ogre::Item* getItemFromResource(const std::string& name, Ogre::SceneManager* smgr) override;
		Ogre::Item* getItemFromFileSystem(const std::string& fileName, Ogre::SceneManager* smgr) override;
		ItemAndTransforms getItemAndTransformsFromResource(const std::string& name, Ogre::SceneManager* smgr) override;
		MeshAndDataBlock getMeshFromResource(const std::string& name) override;
		MeshAndDataBlock getMeshFromFileSystem(const std::string& name) override;

		///Deleted copy constructor
		glTFLoader(const glTFLoader&) = delete;

		///Deleted assignment operator
		glTFLoader& operator=(const glTFLoader&) = delete;
	};
}

//To facilitate the use of the library:
#include "Ogre_glTF_OgreResource.hpp"
#include "Ogre_glTF_OgrePlugin.hpp"
