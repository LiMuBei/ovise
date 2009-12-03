#include "../OViSE/SelectionManager.h"

// Singleton
SelectionManager* SelectionManager::instance = 0;
SelectionManager* SelectionManager::getSingletonPtr()
{
	if (SelectionManager::instance == 0) SelectionManager::instance = new SelectionManager();
	return SelectionManager::instance;
}

SelectionManager::SelectionManager() { this->Valid = true; }
SelectionManager::~SelectionManager(void) { }
// General
// wxPropertyGrid-Handling
bool SelectionManager::IsValid() { return this->Valid; }
bool SelectionManager::IsSelected(QualifiedName qName)
{
	if (this->Selection.IsEmpty()) return false;
	for(unsigned long IT = 0; IT < this->Selection.Count(); IT++)
	{
		if (this->Selection[IT].UniqueName().IsSameAs(qName.UniqueName())) return true;
	}
	return false;
}
bool SelectionManager::AddNOTIMPLEMENTEDToPGCategory(wxPropertyCategory* PC)
{
	if ( PC == 0 ) return false;

	wxPropertyGrid* PG = PC->GetGrid();
	if ( PG == 0 ) return false;

	// Info
	wxPGProperty* PInfo = PG->AppendIn(PC, new wxStringProperty(wxT("INFO"), wxPG_LABEL));
	PG->SetPropertyValue(PInfo, ToWxString("not implemented"));
	PG->DisableProperty(PInfo);

	return true;
}
bool SelectionManager::AddCameraToPGCategory(wxPropertyCategory* PC, Ogre::Camera* C)
{
	/*
	C->setAspectRatio(); //+
	C->setAutoAspectRatio(); //?

	C->setCastShadows();


	C->setDirection(); //+
	C->setFarClipDistance(); //+

	C->setFocalLength();
	C->setFOVy(); //+

	C->setLodBias(); //+

	C->setNearClipDistance();

	C->setPolygonMode(); //+
	C->setPolygonModeOverrideable(); //?

	C->setProjectionType(); //+

	C->setRenderingDistance();

	C->setUseRenderingDistance(); //+

	C->setVisible();
*/

	return true;
}
bool SelectionManager::AddEntityToPGCategory(Ogre::Entity* E)
{
	// Validate parameters
	if ( E == 0 ) return false;

	// Validate members
	if ( this->PG == 0 ) return false;
	
	// Get QualifiedName
	QualifiedName qEntity = OgreMediator::GetSingletonPtr()->GetObjectAccess()->GetQualifiedNameOfObject(ToWxString(E->getName()));
	if ( !OgreMediator::GetSingletonPtr()->iEntity.Exist(qEntity) ) return false;

	// Handle recusive processing of SceneNode
	wxPropertyCategory* PCParent = this->AddSceneNodeToPGCategory((Ogre::SceneNode*)E->getParentNode());
	if (PCParent == 0) return false;

	// Perpare ID and Label
	wxString Label, CategoryID, PropertyID;
	CategoryID = ToWxString(E->getName());

	// Add cathergory to grid
	Label = ToWxString("Entity: '");
	Label << ToWxString(E->getName());
	Label << ToWxString("'");
	
	wxPropertyCategory* PCEntity = new wxPropertyCategory(Label, CategoryID);
	this->PG->AppendIn(PCParent, PCEntity);
	PCEntity->SetExpanded(false);

	// Name
	Label = ToWxString("Name");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCEntity, new wxStringProperty(Label, PropertyID));
	this->PG->SetPropertyValue(PropertyID, qEntity.UniqueName());
	this->PG->DisableProperty(PropertyID);

	// Native name
	Label = ToWxString("Native");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCEntity, new wxStringProperty(Label, PropertyID));
	this->PG->SetPropertyValue(PropertyID, qEntity.NativeName());
	this->PG->DisableProperty(PropertyID);

	// Generic hint
	Label = ToWxString("Hint");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCEntity, new wxStringProperty(Label, PropertyID));
	this->PG->SetPropertyValue(PropertyID, qEntity.GenericHint());
	this->PG->DisableProperty(PropertyID);

	// Generic name
	Label = ToWxString("Generic");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCEntity, new wxStringProperty(Label, PropertyID));
	this->PG->SetPropertyValue(PropertyID, qEntity.GenericName());
	this->PG->DisableProperty(PropertyID);

	// Meshfile
	Label = ToWxString("Mesh");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCEntity, new wxStringProperty(Label, PropertyID));
	this->PG->SetPropertyValue(PropertyID, E->getMesh()->getName());
	this->PG->DisableProperty(PropertyID);

	// CastShadows
	Label = ToWxString("CastShadows");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCEntity, new wxBoolProperty(Label, PropertyID, E->getCastShadows()));

	// DisplaySkeleton
	Label = ToWxString("DisplaySkeleton");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCEntity, new wxBoolProperty(Label, PropertyID, E->getDisplaySkeleton()));

	return true;
}
bool SelectionManager::AddLightToPGCategory(wxPropertyCategory* PC, Ogre::Light* L)
{
	return true;
}
wxPropertyCategory* SelectionManager::AddSceneNodeToPGCategory(Ogre::SceneNode* SN)
{
	// Validate parameters
	if ( SN == 0 ) return 0;

	// Validate members
	if ( this->PG == 0 ) return false;

	// Get Ogre::SceneManager
	QualifiedName qSceneManager = OgreMediator::GetSingletonPtr()->iSceneManager.GetActiveSceneManager();
	Ogre::SceneManager* SM = OgreMediator::GetSingletonPtr()->iSceneManager.GetPtr(qSceneManager);
	if ( SM == 0 ) return 0;

	// Select PCParent by condition: is Ogre::SceneNode the RootSceneNode?
	wxPropertyCategory* PCParent = 0;
	QualifiedName qSceneNode;
	bool isRoot = false;
	if (SN == SM->getRootSceneNode())
	{
		// Select SceneManager-category as PCParent
		PCParent = (wxPropertyCategory*)this->PG->GetProperty(ToWxString("SceneManager"));
		isRoot = true;
	}
	else
	{
		// Get QualifiedName
		QualifiedName qSceneNode = OgreMediator::GetSingletonPtr()->GetObjectAccess()->GetQualifiedNameOfObject(ToWxString(SN->getName()));
		if ( !OgreMediator::GetSingletonPtr()->iSceneNode.Exist(qSceneNode) ) return 0;

		// Handle recusive processing of SceneNode
		PCParent = this->AddSceneNodeToPGCategory(SN->getParentSceneNode());
	}

	// Stop, if PCParent is NULL
	if (PCParent == 0) return 0;

	// Perpare ID and Label
	wxString Label, CategoryID, PropertyID;
	CategoryID = ToWxString(SN->getName());
	wxPropertyCategory* PCSceneNode = 0;
	
	// Stop, if category already exists and return it
	PCSceneNode = (wxPropertyCategory*)this->PG->GetProperty(CategoryID);
	if (PCSceneNode != 0) return PCSceneNode;

	// Add cathergory to grid
	Label = ToWxString("SceneNode: '");
	Label << ToWxString(SN->getName());
	Label << ToWxString("'");
	
	PCSceneNode = new wxPropertyCategory(Label, CategoryID);
	this->PG->AppendIn(PCParent, PCSceneNode);
	PCSceneNode->SetExpanded(false);

	// Name
	Label = ToWxString("Name");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCSceneNode, new wxStringProperty(Label, PropertyID));
	this->PG->SetPropertyValue(PropertyID, ToWxString(SN->getName()));
	this->PG->DisableProperty(PropertyID);

	// Native name
	Label = ToWxString("Native");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCSceneNode, new wxStringProperty(Label, PropertyID));
	if (isRoot) this->PG->SetPropertyValue(PropertyID, ToWxString(""));
	else this->PG->SetPropertyValue(PropertyID, qSceneManager.NativeName());
	this->PG->DisableProperty(PropertyID);

	// Generic hint
	Label = ToWxString("Hint");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCSceneNode, new wxStringProperty(Label, PropertyID));
	if (isRoot) this->PG->SetPropertyValue(PropertyID, ToWxString(""));
	else this->PG->SetPropertyValue(PropertyID, qSceneManager.GenericHint());
	this->PG->DisableProperty(PropertyID);

	// Generic name
	Label = ToWxString("Generic");
	PropertyID = CategoryID + ToWxString(".") + Label;

	this->PG->AppendIn(PCSceneNode, new wxStringProperty(Label, PropertyID));
	if (isRoot) this->PG->SetPropertyValue(PropertyID, ToWxString(""));
	else this->PG->SetPropertyValue(PropertyID, qSceneManager.GenericName());
	this->PG->DisableProperty(PropertyID);

	// BoundingBox
	Label = ToWxString("Show BoundingBox");
	PropertyID = CategoryID + ToWxString(".") + Label;

	wxPGProperty* PShowBoundingBox = PG->AppendIn(PCSceneNode, new wxBoolProperty(Label, PropertyID, SN->getShowBoundingBox()));

	// Position
	Label = ToWxString("Position");
	PropertyID = CategoryID + ToWxString(".") + Label;

	wxPGProperty* PPosition = PG->AppendIn(PCSceneNode, new wxStringProperty(Label, PropertyID, wxT("<composed>")));
	wxPGProperty* PPositionX = PG->AppendIn(PPosition, new wxFloatProperty(wxT("x"), wxT("px")));
	this->PG->SetPropertyValidator(PPositionX, wxTextValidator(wxFILTER_NUMERIC));
	wxPGProperty* PPositionY = PG->AppendIn(PPosition, new wxFloatProperty(wxT("y"), wxT("py")));
	this->PG->SetPropertyValidator(PPositionY, wxTextValidator(wxFILTER_NUMERIC));
	wxPGProperty* PPositionZ = PG->AppendIn(PPosition, new wxFloatProperty(wxT("z"), wxT("pz")));
	this->PG->SetPropertyValidator(PPositionZ, wxTextValidator(wxFILTER_NUMERIC));
	PPosition->SetExpanded(false);

	// Set values
	this->PG->SetPropertyValue(PPositionX, (double)SN->getPosition().x);
	this->PG->SetPropertyValue(PPositionY, (double)SN->getPosition().y);
	this->PG->SetPropertyValue(PPositionZ, (double)SN->getPosition().z);

	// Rotation
	Label = ToWxString("Rotation");
	PropertyID = CategoryID + ToWxString(".") + Label;

	wxPGProperty* PRotation = PG->AppendIn(PCSceneNode, new wxStringProperty(Label, PropertyID, wxT("<composed>")));
	wxPGProperty* PRotationRoll = PG->AppendIn(PRotation, new wxFloatProperty(wxT("(x-Axis) Roll"), wxT("roll")));
	this->PG->SetPropertyValidator(PRotationRoll, wxTextValidator(wxFILTER_NUMERIC));
	wxPGProperty* PRotationPitch = PG->AppendIn(PRotation, new wxFloatProperty(wxT("(y-Axis) Pitch"), wxT("pitch")));
	this->PG->SetPropertyValidator(PRotationPitch, wxTextValidator(wxFILTER_NUMERIC));
	wxPGProperty* PRotationYaw = PG->AppendIn(PRotation, new wxFloatProperty(wxT("(z-Axis) Yaw"), wxT("yaw")));
	this->PG->SetPropertyValidator(PRotationYaw, wxTextValidator(wxFILTER_NUMERIC));
	PRotation->SetExpanded(false);

	// Set values
	this->PG->SetPropertyValue(PRotationRoll, (double)SN->getOrientation().getPitch().valueDegrees());
	this->PG->SetPropertyValue(PRotationPitch, (double)SN->getOrientation().getRoll().valueDegrees());
	this->PG->SetPropertyValue(PRotationYaw, (double)SN->getOrientation().getYaw().valueDegrees());

	// Scale
	Label = ToWxString("Scale");
	PropertyID = CategoryID + ToWxString(".") + Label;

	wxPGProperty* PScale = PG->AppendIn(PCSceneNode, new wxStringProperty(Label, PropertyID, wxT("<composed>")));
	wxPGProperty* PScaleX = PG->AppendIn(PScale, new wxFloatProperty(wxT("x"), wxT("sx")));
	this->PG->SetPropertyValidator(PScaleX, wxTextValidator(wxFILTER_NUMERIC));
	wxPGProperty* PScaleY = PG->AppendIn(PScale, new wxFloatProperty(wxT("y"), wxT("sy")));
	this->PG->SetPropertyValidator(PScaleY, wxTextValidator(wxFILTER_NUMERIC));
	wxPGProperty* PScaleZ = PG->AppendIn(PScale, new wxFloatProperty(wxT("z"), wxT("sz")));
	this->PG->SetPropertyValidator(PScaleZ, wxTextValidator(wxFILTER_NUMERIC));
	PScale->SetExpanded(false);

	// Set values
	this->PG->SetPropertyValue(PScaleX, (double)SN->getScale().x);
	this->PG->SetPropertyValue(PScaleY, (double)SN->getScale().y);
	this->PG->SetPropertyValue(PScaleZ, (double)SN->getScale().z);
	
	return PCSceneNode;
}
bool SelectionManager::AddMovableObjectToPG(wxPropertyCategory* PCParent, Ogre::MovableObject* MO)
{
	// Validate parameters...
	if ( PCParent == 0 ) return false;
	if ( MO == 0 ) return false;

	// Store wxPropertryGrid (PG is never created in SelectionManager, so it should not be deleted too.
	this->PG = PG;

	// Get QualifiedName
	QualifiedName qMovableObject = QualifiedName::GetQualifiedNameByUnique(ToWxString(MO->getName()));
	if ( !qMovableObject.IsValid() ) return false;

	// Get Ogre::MovableObject-Type
	OgreEnums::MovableObject::MovableType EnumMT = OgreMediator::GetSingletonPtr()->GetObjectAccess()->GetMovableType(qMovableObject);

	// Setup category
	wxString CategoryHeadline;
	switch(EnumMT)
	{
		case OgreEnums::MovableObject::MOVABLETYPE_BillBoardChain:			CategoryHeadline << ToWxString("BillBoardChain : '");		break;
		case OgreEnums::MovableObject::MOVABLETYPE_BillboardSet:			CategoryHeadline << ToWxString("BillboardSet : '");			break;
		case OgreEnums::MovableObject::MOVABLETYPE_Camera:					CategoryHeadline << ToWxString("Camera : '");				break;
		case OgreEnums::MovableObject::MOVABLETYPE_Entity:					CategoryHeadline << ToWxString("Entity : '");				break;
		case OgreEnums::MovableObject::MOVABLETYPE_Frustum:					CategoryHeadline << ToWxString("Frustum : '");				break;
		case OgreEnums::MovableObject::MOVABLETYPE_InstancedGeometry_BatchInstance: CategoryHeadline << ToWxString("InstancedGeometry.BatchInstance : '"); break;
		case OgreEnums::MovableObject::MOVABLETYPE_Light:					CategoryHeadline << ToWxString("Light : '");				break;
		case OgreEnums::MovableObject::MOVABLETYPE_ManualObject:			CategoryHeadline << ToWxString("ManualObject : '");			break;
		case OgreEnums::MovableObject::MOVABLETYPE_MovablePlane:			CategoryHeadline << ToWxString("MovablePlane : '");			break;
		case OgreEnums::MovableObject::MOVABLETYPE_ParticleSystem:			CategoryHeadline << ToWxString("ParticleSystem : '");		break;
		case OgreEnums::MovableObject::MOVABLETYPE_RibbonTrail:				CategoryHeadline << ToWxString("RibbonTrail : '");			break;
		case OgreEnums::MovableObject::MOVABLETYPE_SimpleRenderable:		CategoryHeadline << ToWxString("SimpleRenderable : '");		break;
		case OgreEnums::MovableObject::MOVABLETYPE_StaticGeometry_Region:	CategoryHeadline << ToWxString("StaticGeometry.Region : '");break;

		default:
			CategoryHeadline << ToWxString("MovableObject : '") << qMovableObject.UniqueName() << ToWxString("'");
			return false;
			break;
	}
	CategoryHeadline << qMovableObject.UniqueName() << ToWxString("'");

	// Setup properties of category
	//wxPropertyCategory* PCMovableObject = new wxPropertyCategory(CategoryHeadline, ToWxString(MO->getName()));
	//this->PG->AppendIn(PCParent, PCMovableObject);

	// ...Ogre::MovableObject-part
	switch(EnumMT)
	{
		case OgreEnums::MovableObject::MOVABLETYPE_BillBoardChain:			this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_BillboardSet:			this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_Camera:					this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_Entity:					this->AddEntityToPGCategory((Ogre::Entity*)MO); break;
		case OgreEnums::MovableObject::MOVABLETYPE_Frustum:					this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_InstancedGeometry_BatchInstance: this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_Light:					this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_ManualObject:			this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_MovablePlane:			this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_ParticleSystem:			this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_RibbonTrail:				this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_SimpleRenderable:		this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		case OgreEnums::MovableObject::MOVABLETYPE_StaticGeometry_Region:	this->AddNOTIMPLEMENTEDToPGCategory(PCParent); break;
		default: this->AddNOTIMPLEMENTEDToPGCategory(PCParent); return false; break;
	}

	// ...Ogre::SceneNode-part
	//this->AddSceneNodeToPGCategory(PC, MO->getParentSceneNode(), qMovableObject);

	PCParent->SetExpanded(false);
	return true;
}
bool SelectionManager::GeneratePropertyGridContentFromSelection(wxPropertyGrid* PG) { return this->GeneratePropertyGridContentFromSelection(PG, this->Selection); }
bool SelectionManager::GeneratePropertyGridContentFromSelection(wxPropertyGrid* PG, wxString UniqueNameOfSelection)
{
	/*
	if (UniqueNameOfSelection.IsEmpty())
	{*/
		return this->GeneratePropertyGridContentFromSelection(PG, this->Selection);
	/*}

	if (this->Selections.count(UniqueNameOfSelection) == 1)
	{
		return this->generatePropertyGridContentFromSelection(PG, this->Selections[UniqueNameOfSelection]);
	}
	
	return false;*/
}
bool SelectionManager::GeneratePropertyGridContentFromSelection(wxPropertyGrid* PG, QualifiedNameCollection Selection)
{
	// Validate parameters...
	if ( PG == 0 ) return false;

	// Store wxPropertryGrid (PG is never created in SelectionManager, so it should not be deleted too.
	this->PG = PG;

	// Clear wxPropertryGrid
	this->PG->Clear();

	// Get Ogre::SceneManager
	QualifiedName qSM = OgreMediator::GetSingletonPtr()->iSceneManager.GetActiveSceneManager();
	Ogre::SceneManager* SM = OgreMediator::GetSingletonPtr()->iSceneManager.GetPtr(qSM);
	
	// Validate Ogre::SceneManager...
	if (SM == 0) return false;

	// Add head-cathegory for Ogre::SceneManeger
	wxPropertyCategory* PCSceneManager = new wxPropertyCategory(ToWxString("SceneManager"));
	this->PG->Append(PCSceneManager);
	this->PG->Append(new wxStringProperty(ToWxString("Name"), ToWxString("SceneManagerName")));
	this->PG->SetPropertyValue(ToWxString("SceneManagerName"), OgreMediator::GetSingletonPtr()->iSceneManager.GetActiveSceneManager().UniqueName());
	this->PG->DisableProperty(ToWxString("SceneManagerName"));
	PCSceneManager->SetExpanded(true);

	// Add cathergory "SceneNode: Root"
	this->AddSceneNodeToPGCategory(SM->getRootSceneNode());

	// Create (sub-)cathegories for selected objects (recursiv walktrought)
	if (!Selection.IsEmpty())
	{
		for ( unsigned long IT = 0; IT < Selection.Count(); IT++ )
		{
			Ogre::MovableObject* MO = OgreMediator::GetSingletonPtr()->iMovableObject.GetPtr(Selection[IT]);
			this->AddMovableObjectToPG(PCSceneManager, MO);
		}
	}

	return true;
}
// Handle wxMain->OnPropertyChanged(...) wxPropertyGridEvent
bool SelectionManager::HandlePropertyChanged(wxPGProperty* ChangedProperty)
{
	// It may be NULL
    if ( ChangedProperty == 0 ) return false;

    // New property change
	this->ChangedProperty = ChangedProperty;
	
	// Get name of changed property
	wxString PName = ChangedProperty->GetName();//zAxis_Entity_0.Rotation.rz
	wxStringTokenizer StrTok(PName, ToWxString("."));

	// Prepare returnvalue
	bool ReturnValue = false;

	// Handle object id
	if ( StrTok.HasMoreTokens() )
	{
		// STAGE 1: Get UniqueName and rebuild rest of PropertyID
		wxString UniqueName = StrTok.GetNextToken();
		wxString subPGID;
		while(StrTok.HasMoreTokens())
		{
			subPGID << StrTok.GetNextToken();
			if (StrTok.HasMoreTokens()) subPGID << ToWxString(".");
		}
		
		// STAGE 2: Get QualifiedNames by UniqueName
		QualifiedName qName = OgreMediator::GetSingletonPtr()->GetObjectAccess()->GetQualifiedNameOfObject(UniqueName);
		if ( !qName.IsValid() ) return false;

		// STAGE 3: Get Type by indentified QualifiedName
		OgreEnums::MovableObject::MovableType Type = OgreMediator::GetSingletonPtr()->GetObjectAccess()->GetMovableType(qName);

		// STAGE 4: Futher calls depend on type...
		bool Match = false;
		switch(Type)
		{
		case OgreEnums::MovableObject::MOVABLETYPE_Invalid:
			// STAGE 4.x: Object is no Ogre::MovableObject. Could be Ogre::SceneManager or Ogre::SceneNode
			
			if ((!Match) && OgreMediator::GetSingletonPtr()->iSceneManager.Exist(qName))
			{
				// Got Ogre::SceneManager
				this->IsValid(); // DEBUG: Handling of that type not implemented yet !!!
				Match = false;
			}
			if ((!Match) && OgreMediator::GetSingletonPtr()->iSceneNode.Exist(qName))
			{
				// Got Ogre::SceneNode
				this->HandleSceneNodeChanged(qName, subPGID);
				Match = false;
			}	
			break;

		case OgreEnums::MovableObject::MOVABLETYPE_Camera:
			Match = false; // DEBUG: Handling of that type not implemented yet !!!
			break;

		case OgreEnums::MovableObject::MOVABLETYPE_Entity:	ReturnValue = this->HandleEntityChanged(qName, subPGID); break;

		case OgreEnums::MovableObject::MOVABLETYPE_Light:
			Match = false; // DEBUG: Handling of that type not implemented yet !!!
			break;

		default:
			// DEBUG: Handling of that type not implemented yet !!!
			Match = false;
			break;
		}
	}

	return ReturnValue;
}
bool SelectionManager::HandleSceneNodeChanged(QualifiedName qSceneNode, wxString subPGID)
{
	// Verify qSceneNode
	if (!OgreMediator::GetSingletonPtr()->iSceneNode.Exist(qSceneNode)) return false;
	Ogre::SceneNode* SN = OgreMediator::GetSingletonPtr()->iSceneNode.GetPtr(qSceneNode);
	return this->HandleSceneNodeChanged(SN, subPGID);
}
bool SelectionManager::HandleSceneNodeChanged(Ogre::SceneNode* SN, wxString subPGID)
{
	// INFO: Don't worry. Change parts of composed-types will be propagated :-)

	// It may be NULL
	if ( SN == 0 ) return false;

	wxVariant value = this->ChangedProperty->GetValue();  // TODO: obsolete because it's now accessible in object-member

	// Get name of changed property
	wxStringTokenizer StrTok(subPGID, ToWxString("."));
	
	// STAGE 2: Handle attribute
	wxString AttributeID = StrTok.GetNextToken();

	if (AttributeID.IsSameAs(ToWxString("Position")))
	{
		if (!StrTok.HasMoreTokens()) this->HandlePositionChanged(SN);
		return true;
	}
	if (AttributeID.IsSameAs(ToWxString("Rotation")))
	{
		if (!StrTok.HasMoreTokens()) this->HandleRotationChanged(SN);
		return true;
	}
	if (AttributeID.IsSameAs(ToWxString("Scale")))
	{
		if (!StrTok.HasMoreTokens()) this->HandleScaleChanged(SN);
		return true;
	}
	if (AttributeID.IsSameAs(ToWxString("Name"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("Native"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("Hint"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("Generic"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("Show BoundingBox")))
	{
		SN->showBoundingBox(value.GetBool());
		return true;
	}

	return false;
}
bool SelectionManager::HandleEntityChanged(QualifiedName qEntity, wxString subPGID)
{
	// Verify qEntity
	if (!OgreMediator::GetSingletonPtr()->iEntity.Exist(qEntity)) return false;
	Ogre::Entity* E = OgreMediator::GetSingletonPtr()->iEntity.GetPtr(qEntity);
	return this->HandleEntityChanged(E, subPGID);
}
bool SelectionManager::HandleEntityChanged(Ogre::Entity* E, wxString subPGID)
{
	// INFO: Don't worry. Change parts of composed-types will be propagated :-)

	// It may be NULL
	if ( E == 0 ) return false;

	wxVariant value = this->ChangedProperty->GetValue();  // TODO: obsolete because it's now accessible in object-member

	// Get name of changed property
	wxStringTokenizer StrTok(subPGID, ToWxString("."));
	
	// STAGE 2: Handle attribute
	wxString AttributeID = StrTok.GetNextToken();

	if (AttributeID.IsSameAs(ToWxString("Name"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("Native"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("Hint"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("Generic"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("Mesh"))) // not editable
	{
		return false;
	}
	if (AttributeID.IsSameAs(ToWxString("CastShadows")))
	{
		E->setCastShadows(value.GetBool());
		return true;
	}
	if (AttributeID.IsSameAs(ToWxString("DisplaySkeleton")))
	{
		E->setDisplaySkeleton(value.GetBool());
		return true;
	}

	return false;
}

bool SelectionManager::HandlePositionChanged(Ogre::SceneNode* SN)
{
	// It may be NULL
	if ( SN == 0 ) return false;

	// Perpare logging
	wxString MSG;

	MSG = ToWxString("Changing position of Ogre::SceneNode '");
	MSG << ToWxString(SN->getName()) << "':";
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Use variant as string an tokenize it
	wxStringTokenizer SemiSeperator(this->ChangedProperty->GetValue().GetString(), wxT(";"));
	
	// Format subtokens into Ogre::Degree(double) via variant
	wxVariant V;
	V = SemiSeperator.GetNextToken();
	Ogre::Real NewX = Ogre::Real(V.GetDouble());
	V = SemiSeperator.GetNextToken();
	Ogre::Real NewY = Ogre::Real(V.GetDouble());
	V = SemiSeperator.GetNextToken();
	Ogre::Real NewZ = Ogre::Real(V.GetDouble());

	// Prepare values
	Ogre::Vector3 P = SN->getPosition();
	Ogre::Real X = P.x;
	Ogre::Real Y = P.y;
	Ogre::Real Z = P.z;

	// Log before
	MSG = ToWxString("From: X='");
	MSG << (double)X << ToWxString("' | Y='");
	MSG << (double)Y << ToWxString("' | Z='");
	MSG << (double)Z << ToWxString("'");
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Apply change
	SN->setPosition(NewX, NewY, NewZ);
	
	// Reflect values
	P = SN->getPosition();
	X = P.x;
	Y = P.y;
	Z = P.z;

	// Log after
	MSG = ToWxString("To: X='");
	MSG << (double)X << ToWxString("' | Y='");
	MSG << (double)Y << ToWxString("' | Z='");
	MSG << (double)Z << ToWxString("'");
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Update related values 
	wxPGProperty* ChildProperty;
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("px"));
	this->PG->SetPropertyValue(ChildProperty, (double)X);
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("py"));
	this->PG->SetPropertyValue(ChildProperty, (double)Y);
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("pz"));
	this->PG->SetPropertyValue(ChildProperty, (double)Z);

	return true;
}
bool SelectionManager::HandleScaleChanged(Ogre::SceneNode* SN)
{
	// It may be NULL
	if ( SN == 0 ) return false;

	// Perpare logging
	wxString MSG;

	MSG = ToWxString("Changing scale of Ogre::SceneNode '");
	MSG << ToWxString(SN->getName()) << "':";
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Use variant as string an tokenize it
	wxStringTokenizer SemiSeperator(this->ChangedProperty->GetValue().GetString(), wxT(";"));
	
	// Format subtokens into Ogre::Degree(double) via variant
	wxVariant V;
	V = SemiSeperator.GetNextToken();
	Ogre::Real NewX = Ogre::Real(V.GetDouble());
	V = SemiSeperator.GetNextToken();
	Ogre::Real NewY = Ogre::Real(V.GetDouble());
	V = SemiSeperator.GetNextToken();
	Ogre::Real NewZ = Ogre::Real(V.GetDouble());

	// Prepare values
	Ogre::Vector3 P = SN->getScale();
	Ogre::Real X = P.x;
	Ogre::Real Y = P.y;
	Ogre::Real Z = P.z;

	// Log before
	MSG = ToWxString("From: X='");
	MSG << (double)X << ToWxString("' | Y='");
	MSG << (double)Y << ToWxString("' | Z='");
	MSG << (double)Z << ToWxString("'");
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Apply change
	SN->setScale(NewX, NewY, NewZ);
	
	// Reflect values
	P = SN->getScale();
	X = P.x;
	Y = P.y;
	Z = P.z;

	// Log after
	MSG = ToWxString("To: X='");
	MSG << (double)X << ToWxString("' | Y='");
	MSG << (double)Y << ToWxString("' | Z='");
	MSG << (double)Z << ToWxString("'");
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Update related values 
	wxPGProperty* ChildProperty;
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("sx"));
	this->PG->SetPropertyValue(ChildProperty, (double)X);
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("sy"));
	this->PG->SetPropertyValue(ChildProperty, (double)Y);
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("sz"));
	this->PG->SetPropertyValue(ChildProperty, (double)Z);

	return true;
}
bool SelectionManager::HandleRotationChanged(Ogre::SceneNode* SN)
{
	// It may be NULL
	if ( SN == 0 ) return false;

	// Perpare logging
	wxString MSG;

	MSG = ToWxString("Changing rotation of Ogre::SceneNode '");
	MSG << ToWxString(SN->getName()) << "':";
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Use variant as string an tokenize it
	wxStringTokenizer SemiSeperator(this->ChangedProperty->GetValue().GetString(), wxT(";"));
	
	// Format subtokens into Ogre::Degree(double) via variant
	wxVariant V;
	V = SemiSeperator.GetNextToken();
	Ogre::Degree NewR = Ogre::Degree(V.GetDouble());
	V = SemiSeperator.GetNextToken();
	Ogre::Degree NewP = Ogre::Degree(V.GetDouble());
	V = SemiSeperator.GetNextToken();
	Ogre::Degree NewY = Ogre::Degree(V.GetDouble());

	// Prepare values
	Ogre::Quaternion Q = SN->getOrientation();
	Ogre::Degree R = Ogre::Degree(Q.getRoll(false));
	Ogre::Degree P = Ogre::Degree(Q.getPitch(false));
	Ogre::Degree Y = Ogre::Degree(Q.getYaw(false));

	// Log before
	MSG = ToWxString("From: Roll='");
	MSG << (double)R.valueDegrees() << ToWxString("' | Pitch='");
	MSG << (double)P.valueDegrees() << ToWxString("' | Yaw='");
	MSG << (double)Y.valueDegrees() << ToWxString("'");
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Apply change
	SN->setOrientation(Ogre::Quaternion::IDENTITY);
	SN->roll(Ogre::Radian(NewR), Ogre::Node::TS_PARENT);
	SN->pitch(Ogre::Radian(NewP), Ogre::Node::TS_PARENT);
	SN->yaw(Ogre::Radian(NewY), Ogre::Node::TS_PARENT);
	
	// Reflect values
	Q = SN->getOrientation();
	R = Ogre::Degree(Q.getRoll(false));
	P = Ogre::Degree(Q.getPitch(false));
	Y = Ogre::Degree(Q.getYaw(false));

	// Log after
	MSG = ToWxString("To: Roll='");
	MSG << (double)R.valueDegrees() << ToWxString("' | Pitch='");
	MSG << (double)P.valueDegrees() << ToWxString("' | Yaw='");
	MSG << (double)Y.valueDegrees() << ToWxString("'");
	Logging::GetSingletonPtr()->WriteToOgreLog(MSG, Logging::Normal);

	// Update related values 
	wxPGProperty* ChildProperty;
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("roll"));
	this->PG->SetPropertyValue(ChildProperty, (double)P.valueDegrees());
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("pitch"));
	this->PG->SetPropertyValue(ChildProperty, (double)R.valueDegrees());
	ChildProperty = this->ChangedProperty->GetPropertyByName(ToWxString("yaw"));
	this->PG->SetPropertyValue(ChildProperty, (double)Y.valueDegrees());

	return true;
}

// OLD code
	/*


	std::string objname = (const char*)mObjectProperties->GetPropertyValueAsString(wxT("MeshName")).ToAscii();

	OViSESelectionMap selObjs = OViSESceneHandling::getSingletonPtr()->getSelectedObjects();
	if(selObjs.empty())
		return;
	Ogre::SceneNode *snode = selObjs[objname]->getParentSceneNode();
	if(!snode)
		return;
	Ogre::Vector3 pos = snode->getPosition();
	Ogre::Vector3 scale = snode->getScale();
	Ogre::Quaternion tmprot = snode->getOrientation();

	wxVariant tmp = prop->GetValue();
	wxString tmpstr = tmp.GetString();

	double vals[3];

	wxStringTokenizer checker(tmpstr, wxT(";"));

	if(name == wxString(wxT("Translation")))
	{
		for(int i=0; i<3; i++)
		{
			checker.GetNextToken().ToDouble(&vals[i]);
		}
		pos = Ogre::Vector3(vals[0], vals[1], vals[2]);
	}
	if(name == wxString(wxT("Translation.tx")))
	{
		float tx = tmp.GetDouble();
		pos.x = tx;
	}
	if(name == wxString(wxT("Translation.ty")))
	{
		float ty = tmp.GetDouble();
		pos.y = ty;
	}
	if(name == wxString(wxT("Translation.tz")))
	{
		float tz = tmp.GetDouble();
		pos.z = tz;
	}

	snode->setPosition(pos);

	if(name == wxString(wxT("Rotation")))
	{
		for(int i=0; i<3; i++)
		{
			checker.GetNextToken().ToDouble(&vals[i]);
		}
		Ogre::Radian p = Ogre::Radian(Ogre::Degree(vals[0])) - tmprot.getPitch();
		Ogre::Radian r = Ogre::Radian(Ogre::Degree(vals[1])) - tmprot.getRoll();
		Ogre::Radian y = Ogre::Radian(Ogre::Degree(vals[2])) - tmprot.getYaw();
		snode->pitch(p);
		snode->roll(r);
		snode->yaw(y);
	}
	if(name == wxString(wxT("Rotation.rx")))
	{
		float rx = tmp.GetDouble();
		snode->pitch(Ogre::Radian(Ogre::Degree(rx)) - tmprot.getPitch());
	}
	if(name == wxString(wxT("Rotation.ry")))
	{
		float ry = tmp.GetDouble();
		snode->pitch(Ogre::Radian(Ogre::Degree(ry)) - tmprot.getRoll());
	}
	if(name == wxString(wxT("Rotation.rz")))
	{
		float rz = tmp.GetDouble();
		snode->pitch(Ogre::Radian(Ogre::Degree(rz)) - tmprot.getYaw());
	}

	if(name == wxString(wxT("Scale")))
	{
		for(int i=0; i<3; i++)
		{
			checker.GetNextToken().ToDouble(&vals[i]);
		}
		scale = Ogre::Vector3(vals[0], vals[1], vals[2]);
	}
	if(name == wxString(wxT("Scale.sx")))
	{
		float sx = tmp.GetDouble();
		scale.x = sx;
	}
	if(name == wxString(wxT("Scale.sy")))
	{
		float sy = tmp.GetDouble();
		scale.y = sy;
	}
	if(name == wxString(wxT("Scale.sz")))
	{
		float sz = tmp.GetDouble();
		scale.z = sz;
	}

	snode->setScale(scale);

	// call update for scene structure
	mSceneHdlr->showSceneGraphStructure(true);
*/