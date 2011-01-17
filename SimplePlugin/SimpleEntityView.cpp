
#include "SimpleEntityView.h"
#include <boost/lexical_cast.hpp>

SimpleEntityView::SimpleEntityView( Entity* Object, Ogre::SceneManager* Mgr ):
BasicOgreEntityView(Object,Mgr), mNode(0), mOgreEntity(0)
{
	mNode = GetSceneManager()->getRootSceneNode()->createChildSceneNode();

	SyncAttribs(this);
}

SimpleEntityView::~SimpleEntityView()
{
	// detach the scene node and delete it
	mNode->detachObject( mOgreEntity );
	GetSceneManager()->getRootSceneNode()->removeChild(mNode);
	GetSceneManager()->destroySceneNode(mNode);

	// entity is now detached - delete it
	GetSceneManager()->destroyEntity( mOgreEntity );
}

void SimpleEntityView::OnEntityAttributeChanged(
	Entity* Rhs, 
	const std::string& Name,
	const EntityVariantType* Attribute
)
{
	if ( Name == "Position" )
	{
		// Set the position, if it is available
		if ( const vec3* Data = boost::get<vec3>(Attribute) )
			mNode->setPosition( *Data );
	}
	else if ( Name == "Model" )
	{
		// Try to retrieve a model name
		const std::string* ModelName = boost::get<std::string>(
			GetDataEntity()->GetAttribute( "Model" ) );
		
		if ( mOgreEntity )
		{
			mNode->detachObject( mOgreEntity );
			GetSceneManager()->destroyEntity( mOgreEntity );
			mOgreEntity = 0;
		}
		
		// Generate a name for the (OViSE) entity pointer
		const std::string EntityName = "Entity:" + boost::lexical_cast<std::string>( GetDataEntity() );

		if ( ModelName )
			mOgreEntity = GetSceneManager()->createEntity( EntityName, *ModelName );
		else
			mOgreEntity = GetSceneManager()->createEntity( EntityName, Ogre::SceneManager::PT_CUBE );

		mNode->attachObject( mOgreEntity );
	}
}

/** \note This has to be defined as a C function to prevent name mangling.
*/
extern "C" OVISE_DLLAPI
void LoadEntityView( SceneView& View )
{
	View.RegisterView<SimpleEntityView>("Simple");
}