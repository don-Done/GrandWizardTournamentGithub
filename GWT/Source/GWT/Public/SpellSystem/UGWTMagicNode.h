// GWTMagicNode.h
// Core spell functionality node

#pragma once

#include "CoreMinimal.h"
#include "GWTSpellNode.h"
#include "GWTMagicNode.generated.h"

/**
 * Magic node that provides the core spell functionality
 * Handles elemental damage, range, cast time, etc.
 */
UCLASS(BlueprintType)
class GWT_API UGWTMagicNode : public UGWTSpellNode
{
    GENERATED_BODY()
    
public:
    UGWTMagicNode();
    
    // Magic properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Magic")
    float BaseDamage = 10.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Magic")
    float Range = 10.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Magic")
    float CastTime = 1.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Magic")
    float ManaCost = 5.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Magic")
    EGWTElementType ElementType = EGWTElementType::Fire;
    
    // Implementation
    virtual void Execute(UGWTSpellExecutionContext* Context) override;
    virtual EGWTSpellComponentType GetNodeType() const override;
    virtual FString GetNodeTypeAsString() const override;
    
protected:
    // Helper methods for specific elemental effects
    void ApplyFireEffect(AActor* Target, float Damage, UGWTSpellExecutionContext* Context);
    void ApplyIceEffect(AActor* Target, float Damage, UGWTSpellExecutionContext* Context);
    void ApplyLightningEffect(AActor* Target, float Damage, UGWTSpellExecutionContext* Context);
    void ApplyElementalEffect(AActor* Target, float Damage, UGWTSpellExecutionContext* Context);
};