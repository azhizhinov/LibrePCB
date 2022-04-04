/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREPCB_EDITOR_EDITORCOMMANDSET_H
#define LIBREPCB_EDITOR_EDITORCOMMANDSET_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "editorcommand.h"
#include "editorcommandcategory.h"

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {
namespace editor {

/*******************************************************************************
 *  Class EditorCommandSet
 ******************************************************************************/

/**
 * @brief Collection of all commands across all editors
 *
 * @see ::librepcb::editor::EditorCommand
 * @see https://en.wikipedia.org/wiki/Table_of_keyboard_shortcuts
 * @see https://librepcb.discourse.group/t/hotkeys-anyone/229
 */
class EditorCommandSet final {
  Q_DECLARE_TR_FUNCTIONS(EditorCommandSet)

private:
  EditorCommandCategory categoryRoot{"categoryRoot", QString()};

  EditorCommandSet() noexcept {}
  ~EditorCommandSet() noexcept {}

public:
  // General Methods
  static EditorCommandSet& instance() noexcept {
    static EditorCommandSet obj;
    return obj;
  }
  QList<EditorCommandCategory*> getCategories() noexcept {
    return categoryRoot.findChildren<EditorCommandCategory*>();
  }
  QList<EditorCommand*> getCommands(
      const EditorCommandCategory* category) noexcept {
    Q_ASSERT(category);
    return category->findChildren<EditorCommand*>();
  }

  // Workspace
  EditorCommandCategory categoryWorkspace{"categoryWorkspace", tr("Workspace"),
                                          &categoryRoot};
  EditorCommand controlPanel{
      "control_panel",
      tr("Control Panel"),
      tr("Bring the control panel window to front"),
      QIcon(":/img/actions/home.png"),
      {QKeySequence(Qt::Key_Home)},
      &categoryWorkspace,
  };
  EditorCommand libraryManager{
      "library_manager",
      tr("Library Manager") % "...",
      tr("Open the library manager dialog"),
      QIcon(":/img/library/package.png"),
      {QKeySequence(Qt::CTRL + Qt::Key_L)},
      &categoryWorkspace,
  };
  EditorCommand switchWorkspace{
      "switch_workspace",
      tr("Switch Workspace") % "...",
      tr("Choose another workspace to open"),
      QIcon(),
      {},
      &categoryWorkspace,
  };
  EditorCommand workspaceSettings{
      "workspace_settings",
      tr("Workspace Settings") % "...",
      tr("Open the workspace settings dialog"),
      QIcon(":/img/actions/settings.png"),
      {QKeySequence(Qt::CTRL + Qt::Key_Comma)},
      &categoryWorkspace,
  };
  EditorCommand rescanLibraries{
      "rescan_workspace_libraries",
      tr("Rescan Libraries"),
      tr("Rescan all workspace libraries for modifications"),
      QIcon(":/img/actions/refresh.png"),
      {QKeySequence(Qt::Key_F5)},
      &categoryWorkspace,
  };
  EditorCommand closeAllProjects{
      "close_all_projects",
      tr("Close all Projects"),
      tr("Close all currently opened projects"),
      QIcon(":/img/actions/close.png"),
      {QKeySequence(QKeySequence::Close)},
      &categoryWorkspace,
  };
  EditorCommand quit{
      "quit",
      tr("Quit"),
      tr("Close the whole application"),
      QIcon(":/img/actions/quit.png"),
      {QKeySequence(QKeySequence::Quit)},
      &categoryWorkspace,
  };

  // Editor
  EditorCommandCategory categoryEditor{"categoryEditor", tr("Editor"),
                                       &categoryRoot};
  EditorCommand newItem{
      "new_item",
      tr("New"),
      tr("Add a new item"),
      QIcon(":/img/actions/new.png"),
      {QKeySequence(QKeySequence::New)},
      &categoryEditor,
  };
  EditorCommand renameItem{
      "rename_item",
      tr("Rename"),
      tr("Rename selected item(s)"),
      QIcon(":/img/actions/edit.png"),
      {QKeySequence(Qt::Key_F2)},
      &categoryEditor,
  };
  EditorCommand newLibraryElement{
      "new_library_element",
      tr("New Library Element") % "...",
      tr("Create a new library element"),
      QIcon(":/img/actions/new.png"),
      {QKeySequence(QKeySequence::New)},
      &categoryEditor,
  };
  EditorCommand openLibraryElement{
      "open_library_element",
      tr("Open"),
      tr("Open this library element in the editor"),
      QIcon(":/img/actions/edit.png"),
      {},
      &categoryEditor,
  };
  EditorCommand editLibraryElement{
      "edit_library_element",
      tr("Edit"),
      tr("Modify this library element in the editor"),
      QIcon(":/img/actions/edit.png"),
      {},
      &categoryEditor,
  };
  EditorCommand duplicateLibraryElement{
      "duplicate_library_element",
      tr("Duplicate"),
      tr("Create a new element by duplicating this one"),
      QIcon(":/img/actions/clone.png"),
      {},
      &categoryEditor,
  };
  EditorCommand showInFileManager{
      "show_in_file_manager",
      tr("Show in File Manager"),
      tr("Open the directory in the file manager"),
      QIcon(":/img/places/folder.png"),
      {},
      &categoryEditor,
  };
  EditorCommand save{
      "save",
      tr("Save"),
      tr("Save to filesystem"),
      QIcon(":/img/actions/save.png"),
      {QKeySequence(QKeySequence::Save)},
      &categoryEditor,
  };
  EditorCommand newProject{
      "new_project",
      tr("New Project") % "...",
      tr("Create a new project"),
      QIcon(":/img/actions/new.png"),
      {QKeySequence(QKeySequence::New)},
      &categoryEditor,
  };
  EditorCommand openProject{
      "open_project",
      tr("Open Project") % "...",
      tr("Open an existing project"),
      QIcon(":/img/actions/open.png"),
      {QKeySequence(QKeySequence::Open)},
      &categoryEditor,
  };
  EditorCommand closeProject{
      "close_project",
      tr("Close Project"),
      tr("Close the currently opened project"),
      QIcon(":/img/actions/close.png"),
      {QKeySequence(QKeySequence::Close)},
      &categoryEditor,
  };
  EditorCommand saveProject{
      "save_project",
      tr("Save Project"),
      tr("Save the currently opened project"),
      QIcon(":/img/actions/save.png"),
      {QKeySequence(QKeySequence::Save)},
      &categoryEditor,
  };
  EditorCommand projectProperties{
      "project_properties",
      tr("Project Properties") % "...",
      tr("View/modify the project properties"),
      QIcon(),
      {},
      &categoryEditor,
  };
  EditorCommand projectSettings{
      "project_settings",
      tr("Project Settings") % "...",
      tr("View/modify the project settings"),
      QIcon(":/img/actions/settings.png"),
      {QKeySequence(Qt::CTRL + Qt::Key_Comma)},
      &categoryEditor,
  };
  EditorCommand netClasses{
      "net_classes",
      tr("Net Classes") % "...",
      tr("View/modify the net classes"),
      QIcon(),
      {},
      &categoryEditor,
  };
  EditorCommand layerStack{
      "layer_stack",
      tr("Layer Stack") % "...",
      tr("View/modify the board layer stack (number of layers etc.)"),
      QIcon(),
      {},
      &categoryEditor,
  };
  EditorCommand designRules{
      "design_rules",
      tr("Design Rules") % "...",
      tr("View/modify the board design rules"),
      QIcon(),
      {},
      &categoryEditor,
  };
  EditorCommand designRuleCheck{
      "design_rule_check",
      tr("Design Rule Check") % "...",
      tr("Run the design rule check (DRC)"),
      QIcon(":/img/actions/drc.png"),
      {},
      &categoryEditor,
  };
  EditorCommand updateProjectLibrary{
      "update_project_library",
      tr("Update Project Library") % "...",
      tr("Update project library elements from workspace libraries"),
      QIcon(":/img/actions/refresh.png"),
      {},
      &categoryEditor,
  };
  EditorCommand schematicEditor{
      "schematic_editor",
      tr("Schematic Editor"),
      tr("Bring the schematic editor window to front"),
      QIcon(":/img/actions/schematic.png"),
      {},
      &categoryEditor,
  };
  EditorCommand newSheet{
      "new_sheet",
      tr("New Sheet") % "...",
      tr("Add a new sheet to the schematic"),
      QIcon(":/img/actions/new.png"),
      {QKeySequence(QKeySequence::New)},
      &categoryEditor,
  };
  EditorCommand renameSheet{
      "rename_sheet",
      tr("Rename Sheet") % "...",
      tr("Rename the current schematic sheet"),
      QIcon(),
      {QKeySequence(Qt::Key_F2)},
      &categoryEditor,
  };
  EditorCommand boardEditor{
      "board_editor",
      tr("Board Editor"),
      tr("Bring the board editor window to front"),
      QIcon(":/img/actions/board_editor.png"),
      {},
      &categoryEditor,
  };
  EditorCommand newBoard{
      "new_board",
      tr("New Board") % "...",
      tr("Add a new board to the project"),
      QIcon(":/img/actions/new.png"),
      {QKeySequence(QKeySequence::New)},
      &categoryEditor,
  };
  EditorCommand copyBoard{
      "copy_board",
      tr("Copy Board") % "...",
      tr("Add a copy of the current board to the project"),
      QIcon(":/img/actions/copy.png"),
      {},
      &categoryEditor,
  };
  EditorCommand removeBoard{
      "remove_board",
      tr("Remove Board"),
      tr("Remove the current board from the project"),
      QIcon(":/img/actions/delete.png"),
      {},
      &categoryEditor,
  };
  EditorCommand showPlanes{
      "show_planes",
      tr("Show All Planes"),
      tr("Make the filled areas of all planes visible"),
      QIcon(":/img/actions/show_planes.png"),
      {},
      &categoryEditor,
  };
  EditorCommand hidePlanes{
      "hide_planes",
      tr("Hide All Planes"),
      tr("Make the filled areas of all planes invisible"),
      QIcon(":/img/actions/hide_planes.png"),
      {},
      &categoryEditor,
  };
  EditorCommand rebuildPlanes{
      "rebuild_planes",
      tr("Rebuild All Planes"),
      tr("Re-calculate the filled areas of all planes"),
      QIcon(":/img/actions/rebuild_plane.png"),
      {},
      &categoryEditor,
  };

  // Tools
  EditorCommandCategory categoryTools{"categoryTools", tr("Tools"),
                                      &categoryRoot};
  EditorCommand abort{
      "abort",
      tr("Abort Command"),
      tr("Abort the currently active command"),
      QIcon(":/img/actions/stop.png"),
      {QKeySequence(Qt::Key_Escape)},
      &categoryTools,
  };
  EditorCommand toolSelect{
      "tool_select",
      tr("Select"),
      tr("Select & modify existing objects"),
      QIcon(":/img/actions/select.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolLine{
      "tool_line",
      tr("Draw Line"),
      tr("Draw line graphics objects"),
      QIcon(":/img/actions/draw_line.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolRect{
      "tool_rect",
      tr("Draw Rectangle"),
      tr("Draw rectangular graphics objects"),
      QIcon(":/img/actions/draw_rectangle.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolPolygon{
      "tool_polygon",
      tr("Draw Polygon"),
      tr("Draw polygon graphics objects"),
      QIcon(":/img/actions/draw_polygon.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolCircle{
      "tool_circle",
      tr("Draw Circle"),
      tr("Draw circular graphics objects"),
      QIcon(":/img/actions/draw_circle.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolText{
      "tool_text",
      tr("Add Text"),
      tr("Add text graphics objects"),
      QIcon(":/img/actions/add_text.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolName{
      "tool_name",
      tr("Add Name"),
      tr("Add name text graphics objects"),
      QIcon(":/img/actions/add_name.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolValue{
      "tool_value",
      tr("Add Value"),
      tr("Add value text graphics objects"),
      QIcon(":/img/actions/add_value.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolPin{
      "tool_pin",
      tr("Add Pin"),
      tr("Add a pin (electrical connection) to symbol"),
      QIcon(":/img/actions/add_symbol_pin.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolWire{
      "tool_wire",
      tr("Draw Wire"),
      tr("Draw wires to connect symbols"),
      QIcon(":/img/actions/draw_wire.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolNetLabel{
      "tool_netlabel",
      tr("Add Net Label"),
      tr("Add net labels to specify the net of wires"),
      QIcon(":/img/actions/draw_netlabel.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolThtPad{
      "tool_tht_pad",
      tr("Add THT Pad"),
      tr("Add a plated through-hole pad to the footprint"),
      QIcon(":/img/actions/add_tht_pad.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolSmtPad{
      "tool_smt_pad",
      tr("Add SMT Pad"),
      tr("Add a surface mounted pad to the footprint"),
      QIcon(":/img/actions/add_smt_pad.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolVia{
      "tool_via",
      tr("Add Via"),
      tr("Add through-hole vias to the board"),
      QIcon(":/img/actions/add_via.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolTrace{
      "tool_trace",
      tr("Draw Trace"),
      tr("Draw copper traces to interconnect devices"),
      QIcon(":/img/actions/draw_wire.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolPlane{
      "tool_plane",
      tr("Draw Plane"),
      tr("Draw an electrically connected copper area to connect pads and vias"),
      QIcon(":/img/actions/add_plane.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolHole{
      "tool_hole",
      tr("Add Hole"),
      tr("Add non-plated holes (drills)"),
      QIcon(":/img/actions/add_hole.png"),
      {},
      &categoryTools,
  };
  EditorCommand toolComponent{
      "tool_component",
      tr("Add Component") % "...",
      tr("Add components to the schematic"),
      QIcon(":/img/actions/add_component.png"),
      {},
      &categoryTools,
  };

  // Components
  EditorCommandCategory categoryComponents{"categoryComponents",
                                           tr("Components"), &categoryRoot};
  EditorCommand componentResistor{
      "component_resistor",
      tr("Resistor"),
      tr("Add resistors to the schematic"),
      QIcon(":/img/library/resistor_eu.png"),
      {},
      &categoryComponents,
  };
  EditorCommand componentInductor{
      "component_inductor",
      tr("Inductor"),
      tr("Add inductors to the schematic"),
      QIcon(":/img/library/inductor_eu.png"),
      {},
      &categoryComponents,
  };
  EditorCommand componentCapacitorBipolar{
      "component_capacitor_bipolar",
      tr("Bipolar Capacitor"),
      tr("Add bipolar capacitors to the schematic"),
      QIcon(":/img/library/bipolar_capacitor_eu.png"),
      {},
      &categoryComponents,
  };
  EditorCommand componentCapacitorUnipolar{
      "component_capacitor_unipolar",
      tr("Unipolar Capacitor"),
      tr("Add unipolar capacitors to the schematic"),
      QIcon(":/img/library/unipolar_capacitor_eu.png"),
      {},
      &categoryComponents,
  };
  EditorCommand componentGnd{
      "component_gnd",
      tr("GND SUpply"),
      tr("Add GND supply symbols to the schematic"),
      QIcon(":/img/library/gnd.png"),
      {},
      &categoryComponents,
  };
  EditorCommand componentVcc{
      "component_vcc",
      tr("VCC Supply"),
      tr("Add VCC supply symbols to the schematic"),
      QIcon(":/img/library/vcc.png"),
      {},
      &categoryComponents,
  };

  // Import/Export
  EditorCommandCategory categoryImportExport{
      "categoryImportExport", tr("Import/Export"), &categoryRoot};
  EditorCommand print{
      "print",
      tr("Print") % "...",
      tr("Send graphics to printer"),
      QIcon(":/img/actions/print.png"),
      {QKeySequence(QKeySequence::Print)},
      &categoryImportExport,
  };
  EditorCommand importDxf{
      "import_dxf",
      tr("Import DXF") % "...",
      tr("Import 2D mechanical drawing"),
      QIcon(":/img/actions/export_svg.png"),
      {},
      &categoryImportExport,
  };
  EditorCommand importEagleLibrary{
      "import_eagle_library",
      tr("Import EAGLE Library") % "...",
      tr("Import library elements from a EAGLE *.lbr file"),
      QIcon(),
      {},
      &categoryImportExport,
  };
  EditorCommand exportPdf{
      "export_pdf",
      tr("Export PDF") % "...",
      tr("Export graphics as a PDF"),
      QIcon(":/img/actions/pdf.png"),
      {},
      &categoryImportExport,
  };
  EditorCommand exportImage{
      "export_image",
      tr("Export Image") % "...",
      tr("Export graphics as a pixmap"),
      QIcon(":/img/actions/export_pixmap.png"),
      {},
      &categoryImportExport,
  };
  EditorCommand exportLppz{
      "export_lppz",
      tr("Export *.lppz Archive") % "...",
      tr("Export the project as a self-contained *.lppz archive"),
      QIcon(":/img/actions/export_zip.png"),
      {},
      &categoryImportExport,
  };
  EditorCommand generateBom{
      "generate_bom",
      tr("Generate Bill of Materials") % "...",
      tr("Generate bill of materials (BOM) file"),
      QIcon(":/img/actions/generate_bom.png"),
      {},
      &categoryImportExport,
  };
  EditorCommand generateFabricationData{
      "generate_fabrication_data",
      tr("Generate Fabrication Data") % "...",
      tr("Generate Gerber/Excellon files for PCB fabrication"),
      QIcon(":/img/actions/export_gerber.png"),
      {},
      &categoryImportExport,
  };
  EditorCommand generatePickPlace{
      "generate_pick_place",
      tr("Generate Pick&&Place Files") % "...",
      tr("Generate pick&&place files for automated PCB assembly"),
      QIcon(":/img/actions/export_pick_place_file.png"),
      {},
      &categoryImportExport,
  };
  EditorCommand orderPcb{
      "order_pcb",
      tr("Order PCB") % "...",
      tr("Start ordering the PCB online"),
      QIcon(":/img/actions/order_pcb.png"),
      {QKeySequence(Qt::CTRL + Qt::Key_O)},
      &categoryImportExport,
  };

  // Modify
  EditorCommandCategory categoryModify{"categoryModify", tr("Modify"),
                                       &categoryRoot};
  EditorCommand undo{
      "undo",
      tr("Undo"),
      tr("Revert the last modification"),
      QIcon(":/img/actions/undo.png"),
      // Don't use QKeySequence::Undo for consistency with the Redo shortcuts.
      {QKeySequence(Qt::CTRL + Qt::Key_Z)},
      &categoryModify,
  };
  EditorCommand redo{
      "redo",
      tr("Redo"),
      tr("Re-apply the last reverted modification"),
      QIcon(":/img/actions/redo.png"),
      {QKeySequence(Qt::CTRL + Qt::Key_Y),
       QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z)},
      &categoryModify,
  };
  EditorCommand cut{
      "cut",
      tr("Cut"),
      tr("Cut the selected object(s) to clipboard"),
      QIcon(":/img/actions/cut.png"),
      {QKeySequence(QKeySequence::Cut)},
      &categoryModify,
  };
  EditorCommand copy{
      "copy",
      tr("Copy"),
      tr("Copy the selected object(s) to clipboard"),
      QIcon(":/img/actions/copy.png"),
      {QKeySequence(QKeySequence::Copy)},
      &categoryModify,
  };
  EditorCommand paste{
      "paste",
      tr("Paste"),
      tr("Paste object(s) from the clipboard"),
      QIcon(":/img/actions/paste.png"),
      {QKeySequence(QKeySequence::Paste)},
      &categoryModify,
  };
  EditorCommand rotateCcw{
      "rotate_ccw",
      tr("Rotate Counterclockwise"),
      tr("Rotate the selected object(s) counterclockwise"),
      QIcon(":/img/actions/rotate_left.png"),
      {QKeySequence(Qt::Key_R)},
      &categoryModify,
  };
  EditorCommand rotateCw{
      "rotate_cw",
      tr("Rotate Clockwise"),
      tr("Rotate the selected object(s) clockwise"),
      QIcon(":/img/actions/rotate_right.png"),
      {QKeySequence(Qt::SHIFT + Qt::Key_R)},
      &categoryModify,
  };
  EditorCommand mirrorHorizontal{
      "mirror_horizontal",
      tr("Mirror Horizontally"),
      tr("Mirror the selected object(s) horizontally"),
      QIcon(":/img/actions/flip_horizontal.png"),
      {QKeySequence(Qt::Key_M)},
      &categoryModify,
  };
  EditorCommand mirrorVertical{
      "mirror_vertically",
      tr("Mirror Vertically"),
      tr("Mirror the selected object(s) vertically"),
      QIcon(":/img/actions/flip_vertical.png"),
      {QKeySequence(Qt::SHIFT + Qt::Key_M)},
      &categoryModify,
  };
  EditorCommand flipHorizontal{
      "flip_horizontal",
      tr("Flip Horizontally"),
      tr("Flip the selected object(s) horizontally to the other board side"),
      QIcon(":/img/actions/flip_horizontal.png"),
      {QKeySequence(Qt::Key_F)},
      &categoryModify,
  };
  EditorCommand flipVertical{
      "flip_vertically",
      tr("Flip Vertically"),
      tr("Flip the selected object(s) vertically to the other board side"),
      QIcon(":/img/actions/flip_vertical.png"),
      {QKeySequence(Qt::SHIFT + Qt::Key_F)},
      &categoryModify,
  };
  EditorCommand remove{
      "remove",
      tr("Remove"),
      tr("Delete the selected object(s)"),
      QIcon(":/img/actions/delete.png"),
      {QKeySequence(QKeySequence::Delete)},
      &categoryModify,
  };

  // View
  EditorCommandCategory categoryView{"categoryView", tr("View"), &categoryRoot};
  EditorCommand zoomIn{
      "zoom_in",
      tr("Zoom In"),
      tr("Increase zoom level"),
      QIcon(":/img/actions/zoom_in.png"),
      {QKeySequence(QKeySequence::ZoomIn)},
      &categoryView,
  };
  EditorCommand zoomOut{
      "zoom_out",
      tr("Zoom Out"),
      tr("Decrease zoom level"),
      QIcon(":/img/actions/zoom_out.png"),
      {QKeySequence(QKeySequence::ZoomOut)},
      &categoryView,
  };
  EditorCommand zoomFit{
      "zoom_fit",
      tr("Zoom to Fit Contents"),
      tr("Set zoom level to fit the whole content"),
      QIcon(":/img/actions/zoom_all.png"),
      {},
      &categoryView,
  };
  EditorCommand selectAll{
      "select_all",
      tr("Select All"),
      tr("Select all visible objects"),
      QIcon(":/img/actions/select_all.png"),
      {QKeySequence(QKeySequence::SelectAll)},
      &categoryView,
  };
  EditorCommand gridProperties{
      "grid_properties",
      tr("Grid Properties") % "...",
      tr("View/modify the grid properties"),
      QIcon(":/img/actions/grid.png"),
      {},
      &categoryView,
  };

  // Help
  EditorCommandCategory categoryHelp{"categoryHelp", tr("Help"), &categoryRoot};
  EditorCommand onlineDocumentation{
      "online_documentation",
      tr("Online Documentation") % "...",
      tr("Open the documentation in the web browser"),
      QIcon(":/img/actions/help.png"),
      {QKeySequence(QKeySequence::HelpContents)},
      &categoryHelp,
  };
  EditorCommand website{
      "website",
      tr("LibrePCB Website") % "...",
      tr("Open the LibrePCB website in the web browser"),
      QIcon(":/img/actions/open_browser.png"),
      {},
      &categoryHelp,
  };
  EditorCommand aboutLibrePcb{
      "about_librepcb",
      tr("About LibrePCB") % "...",
      tr("Show information about the application"),
      QIcon(":/img/logo/48x48.png"),
      {},
      &categoryHelp,
  };
  EditorCommand aboutQt{
      "about_qt",
      tr("About Qt") % "...",
      tr("Show information about Qt"),
      QIcon(),
      {},
      &categoryHelp,
  };
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb

#endif
