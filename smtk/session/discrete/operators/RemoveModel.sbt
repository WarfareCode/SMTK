<?xml version="1.0" encoding="utf-8" ?>
<!-- Description of the model "CloseModel" Operation -->
<SMTK_AttributeResource Version="3">
  <Definitions>
    <!-- Operation -->
    <include href="smtk/operation/Operation.xml"/>
    <AttDef Type="remove model" BaseType="operation" AdvanceLevel="11">
      <AssociationsDef Name="model(s)" NumberOfRequiredValues="1" Extensible="true" HoldReference="true">
        <Accepts><Resource Name="smtk::session::discrete::Resource" Filter="model"/></Accepts>
      </AssociationsDef>
    </AttDef>

    <!-- Result -->
    <include href="smtk/operation/Result.xml"/>
    <AttDef Type="result(remove model)" BaseType="result">
      <!-- The close models are stored in the base result's "expunged" item. -->
    </AttDef>
  </Definitions>
</SMTK_AttributeResource>
