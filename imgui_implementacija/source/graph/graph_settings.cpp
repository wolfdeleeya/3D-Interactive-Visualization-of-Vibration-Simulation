#include "graph/graph_settings.h"
#include "imgui.h"
#include "custom_imgui_widgets.h"

void BarGraphSettings::draw() { ImGui::DragFloat("Bar Width", &bar_width, 0.05, 0, 1); }

void SubplotsComparisonSettings::draw() 
{
	int num_of_selected_cells = m_engine_data->num_of_selected_cells();

	//if the number of selected cells is greater than 1, than the grid of subplots will have more than 1 subplot
	if (num_of_selected_cells > 1)
		ImGui::DragInt("Number Of Colums", &num_of_columns, 1, 1, num_of_selected_cells);
}

void RelativeComparisonSettings::draw()
{
	std::vector<unsigned int> selected_cells_indeces(m_engine_data->selected_cells());
	unsigned int num_of_selected_cells = selected_cells_indeces.size();

    if (ImGui::TreeNode("Pick Referant Selected Cell"))
    {
        static int selected = -1;
        for (int i = 0; i < num_of_selected_cells; ++i)
        {
            std::string cell_label = "Cell ";
            cell_label += selected_cells_indeces[i];

            glm::vec3 color = m_engine_data->get_color_for_selected_cell(i);
            ImU32 packed_color = ImGui::GetColorU32({ color.r, color.g, color.b, 1 });
            
            if (ImGui::SelectableCustomColor(cell_label.c_str(), selected == i, packed_color, packed_color)) {
                selected = i;
                on_referent_cell_changed.invoke(selected_cells_indeces[i]);
            }
        }
        ImGui::TreePop();
    }
}
