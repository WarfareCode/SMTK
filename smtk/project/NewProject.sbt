<?xml version="1.0" encoding="utf-8" ?>
<!-- Description of the project "NewProject" Operation -->
<SMTK_AttributeResource Version="3">
  <Definitions>
    <AttDef Type="new-project" Label="New Project" BaseType="" Version="1">
      <BriefDescription>
        Create and initialize a CMB project.
      </BriefDescription>
      <DetailedDescription>
        A CMB project stores and manages the data needed to build
        simulation input decks. The initial version of the project
        operator will take as input two files, (i) an Exodus model
        file and (ii) a simulation template (workflow) file.
        All persistent data are stored in a filesystem directory
        that is assigned when the project is created.
      </DetailedDescription>
      <ItemDefinitions>
        <Directory Name="workspace-path" Label="Workspace Directory" NumberOfRequiredValues="1" ShouldExist="true">
          <BriefDescription>Root directory for projects.</BriefDescription>
        </Directory>
        <String Name="project-folder" Label="Name (Subdirectory)" NumberOfRequiredValues="1">
          <BriefDescription>Project files will be stored in a workspace folder with this name.</BriefDescription>
        </String>

        <File Name="simulation-template" Label="Simulation Template File" NumberOfRequiredValues="1"
          ShouldExist="true" Optional="true" IsEnabledByDefault="true"
          FileFilters="CMB Template Files (*.sbt);;All Files (*)">
          <BriefDescription>The CMB template file (*.sbt) specifying the simulation</BriefDescription>
        </File>

        <Group Name="model-group" Label="Input Geometry" Optional="true" IsEnabledByDefault="true">
          <ItemDefinitions>
            <File Name="model-file" Label="Geometry File" NumberOfRequiredValues="1" ShouldExist="true"
              FileFilters="Exodus Files (*.ex? *.gen);;NetCDF Files (*.ncdf);;All Files (*)">
              <BriefDescription>The model or mesh file to import into the project.</BriefDescription>
              <DetailedDescription>The current implementation only supports Exodus and NetCDF files.</DetailedDescription>
            </File>

            <File Name="second-model-file" Label="Second Geometry File" NumberOfRequiredValues="1" ShouldExist="true"
              Optional="true" IsEnabledByDefault="false"
              FileFilters="Exodus Files (*.ex? *.gen);;NetCDF Files (*.ncdf);;All Files (*)">
              <BriefDescription>A second model or mesh file to import into the project.</BriefDescription>
              <DetailedDescription>The current implementation only supports Exodus and NetCDF files.</DetailedDescription>
            </File>

            <Void Name="copy-file" Label="Copy Geometry File(s) Into Project Folder" Optional="true" IsEnabledByDefault="true">
              <BriefDescription>If enabled, store a copy of the file in the project directory.</BriefDescription>
            </Void>

          </ItemDefinitions>
        </Group>
        <!--
          The "use-vtk-session" option is set TRUE by default, as a temporary workaround for
          loading exodus files, which otherwise would use the mesh session. The vtk session is
          used because it copies the side set and element block names to the smtk model.
          Once the mesh session code is updated to do the same thing, recommend that the
          default for this item be changed to FALSE.
        -->
        <Void Name="use-vtk-session" Label="Use VTK Session" AdvanceLevel="1"
          Optional="true" IsEnabledByDefault="true">
          <BriefDescription>Use VTK session for importing model files</BriefDescription>
          <DetailedDescription>
            Use this option to load models with smtk::session::vtk in place of the default
            smtk logic.
          </DetailedDescription>
        </Void>
      </ItemDefinitions>
    </AttDef>
  </Definitions>

    <Views>
      <View Type="Instanced" Title="New Project" TopLevel="true"
        FilterByAdvanceLevel="true" FilterByCategory="false">
        <InstancedAttributes>
          <Att Name="new-project" Type="new-project" />
        </InstancedAttributes>
      </View>
    </Views>
</SMTK_AttributeResource>
