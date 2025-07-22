# Custom target to execute the notebook and generate a PDF
add_custom_target(notebook
  COMMAND jupyter nbconvert --to notebook --execute --inplace performance_analysis.ipynb
  COMMAND jupyter nbconvert --to pdf performance_analysis.ipynb --output performance_analysis.pdf --no-input
  COMMENT "Execute the notebook and generate a PDF."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/plot_utils.py ${CMAKE_CURRENT_SOURCE_DIR}/performance_analysis.ipynb
)

# Custom target to generate figures using a Python script
add_custom_target(generate-figures
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_figures.py
  COMMENT "Generate figures using the Python script."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/plot_utils.py ${CMAKE_CURRENT_SOURCE_DIR}/generate_figures.py
)