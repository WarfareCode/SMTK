Note any non-backward compatible changes are in ***bold italics***.

## Changes to the View System and Qt Extensions

### AttributeItemInfo has been renamed
The new name is qtAttributeItemInfo and it is now located in its own header and cxx files instead of qtItem.h and qtItem.cxx.  **To update existing code all you need do is a name replace.**  qtItem.h includes qtAttributeItemInfo.h so no additional includes are needed.

### Supporting Item Paths for ItemViews
You can now refer to an item via its path relative to its owning Attribute and uses UNIX style.  Item names that could be used as variable names in C++ are supported and have been extended to also include . and -.  The following show several examples:

```xml
  <Views>
    <View Type="Instanced" Title="Grammar Test" TopLevel="true">
      <InstancedAttributes>
        <Att Name="Test Attribute" Type="test">
          <ItemViews>
            <View Item="a" Type="Default" Option="SpinBox"/>
            <View Path="/b" ReadOnly="true"/>
            <View Path="/c/d" FixedWidth="20"/>
            <View Path="/c/e" Option="SpinBox"/>
            <View Path="/f/f-a" Option="SpinBox"/>
            <View Path="/f/f-b" FixedWidth="10"/>
            <View Path="/f/f-c/f.d" FixedWidth="0"/>
            <View Path="/f/f-c/f.e" Option="SpinBox"/>
          </ItemViews>
        </Att>
      </InstancedAttributes>
    </View>
  </Views>
```
In the case of extensible Group Items, the item view style refers to subgroups.  For a complete example, please see viewPathTest.sbt that is located in data/attribute/attribute_collection.

### Changes to qtItem
* **Removed passAdvanceCheck method** - In the qtItem classes, to determine if an item should be viewed, the code was either calling qtBaseView::displayItem check or a combination of qtUIManager::passItemCategoryCheck and this method.  Since a view can have its own set of filtering rules, having this method just confuses the developer as to how to determine if the item should be displayed.

### Changes to qtFileItem
* Invalidity calculation now takes into consideration the ShouldExists hint associated with the item's definition.
* Combobox now only sets the item's value when the user finishes entering the filename instead of every keystroke.

### Changes to qtInputItem
* Added a forceUpdate method - this always forces the object to act as if the underlying item was modified.  Used mainly by helper classes like qtDiscreteValueEditor.

### Changes to qtDiscreteValueEditor
* The modified signal from the corresponding qtInputItem is no longer sent when the underlying ValueItem is modified.  It is now sent after the Editor's internal widgets have been appropriately updated.

### Changes to qtBaseView
* The displayItem test now calls 2 new methods categoryTest and advanceLevelTest.  This makes it easier for derived classes to override the filtering behavior

### Changes to qtAnalysisView
* Now supports Exclusive property for Analysis and Top Level Analyses
* The order of displaying analyses is no longer resorted alphabetically and will be displayed in the order they were defined.
* Overrides the categoryTest method to return always true since its data should never be filtered based on categories.

### Changes to qtUIManager
* The set of categories is being passed to setToLevelCategories is now compared to what was already set.