#ifndef RAZE_SDK_ICLIENTRENDERABLE_H
#define RAZE_SDK_ICLIENTRENDERABLE_H
#pragma once

#include <raze/math.h>
#include <sdk/IClientUnknown.h>

class IPVSNotify;
typedef uint16 ClientShadowHandle_t;
typedef uint16 ClientRenderHandle_t;
typedef uint16 ModelInstanceHandle_t;
struct RenderableInstance_t;
struct model_t;

enum ShadowType_t
{
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC_CUSTOM,
};

class IClientRenderable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual Vector const& GetRenderOrigin() = 0;
	virtual QAngle const& GetRenderAngles() = 0;
	virtual bool ShouldDraw() = 0;
	virtual int GetRenderFlags() = 0;
	virtual void Unused() = 0;
	virtual ClientShadowHandle_t GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t& RenderHandle() = 0;
	virtual const model_t* GetModel() const = 0;
	virtual int DrawModel(int32 flags, const RenderableInstance_t& instance) = 0;
	virtual int	 GetBody() = 0;
	virtual void GetColorModulation(float* color) = 0;
	virtual bool LODTest() = 0;
	virtual bool SetupBones(matrix3x4* pBoneToWorldOut, int32 nMaxBones, int32 boneMask, float currentTime) = 0;
	virtual void SetupWeights(const matrix3x4* pBoneToWorld, int32 nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void DoAnimationEvents() = 0;
	virtual IPVSNotify* GetPVSNotifyInterface() = 0;
	virtual void GetRenderBounds(Vector& mins, Vector& maxs) = 0;
	virtual void GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
	virtual void GetShadowRenderBounds(Vector& mins, Vector& maxs, ShadowType_t shadowType) = 0;
	virtual bool ShouldReceiveProjectedTextures(int32 flags) = 0;
	virtual bool GetShadowCastDistance(float* pDist, ShadowType_t shadowType) const = 0;
	virtual bool GetShadowCastDirection(Vector* pDirection, ShadowType_t shadowType) const = 0;
	virtual bool IsShadowDirty() = 0;
	virtual void MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual ShadowType_t ShadowCastType() = 0;
	virtual void Unused2() = 0;
	virtual void CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t GetModelInstance() = 0;
	virtual const matrix3x4& RenderableToWorldTransform() = 0;
	virtual int LookupAttachment(const char* pAttachmentName) = 0;
	virtual	bool GetAttachment(int32 number, Vector& origin, QAngle& angles) = 0;
	virtual bool GetAttachment(int32 number, matrix3x4& matrix) = 0;
	virtual bool ComputeLightingOrigin(int nAttachmentIndex, Vector modelLightingCenter, const matrix3x4& matrix, Vector& transformedLightingCenter) = 0;
	virtual float* GetRenderClipPlane() = 0;
	virtual int GetSkin() = 0;
	virtual void OnThreadedDrawSetup() = 0;
	virtual bool UsesFlexDelayedWeights() = 0;
	virtual void RecordToolMessage() = 0;
	virtual bool ShouldDrawForSplitScreenUser(int32 nSlot) = 0;
	virtual uint8 OverrideAlphaModulation(uint8 nAlpha) = 0;
	virtual uint8 OverrideShadowAlphaModulation(uint8 nAlpha) = 0;
	virtual IClientModelRenderable* GetClientModelRenderable() = 0;
};

#endif