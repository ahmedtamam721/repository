{
    "cells": [
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "<center>\n    <img src=\"https://gitlab.com/ibm/skills-network/courses/placeholder101/-/raw/master/labs/module%201/images/IDSNlogo.png\" width=\"300\" alt=\"cognitiveclass.ai logo\"  />\n</center>\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "# **Space X  Falcon 9 First Stage Landing Prediction**\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## Assignment:  Machine Learning Prediction\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Estimated time needed: **60** minutes\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Space X advertises Falcon 9 rocket launches on its website with a cost of 62 million dollars; other providers cost upward of 165 million dollars each, much of the savings is because Space X can reuse the first stage. Therefore if we can determine if the first stage will land, we can determine the cost of a launch. This information can be used if an alternate company wants to bid against space X for a rocket launch.   In this lab, you will create a machine learning pipeline  to predict if the first stage will land given the data from the preceding labs.\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "![](https://cf-courses-data.s3.us.cloud-object-storage.appdomain.cloud/IBMDeveloperSkillsNetwork-DS0701EN-SkillsNetwork/api/Images/landing\\_1.gif)\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Several examples of an unsuccessful landing are shown here:\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "![](https://cf-courses-data.s3.us.cloud-object-storage.appdomain.cloud/IBMDeveloperSkillsNetwork-DS0701EN-SkillsNetwork/api/Images/crash.gif)\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Most unsuccessful landings are planed. Space X; performs a controlled landing in the oceans.\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## Objectives\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Perform exploratory  Data Analysis and determine Training Labels\n\n*   create a column for the class\n*   Standardize the data\n*   Split into training data and test data\n\n\\-Find best Hyperparameter for SVM, Classification Trees and Logistic Regression\n\n*   Find the method performs best using test data\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": ""
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "***\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## Import Libraries and Define Auxiliary Functions\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "We will import the following libraries for the lab\n"
        },
        {
            "cell_type": "code",
            "execution_count": 1,
            "metadata": {},
            "outputs": [],
            "source": "# Pandas is a software library written for the Python programming language for data manipulation and analysis.\nimport pandas as pd\n# NumPy is a library for the Python programming language, adding support for large, multi-dimensional arrays and matrices, along with a large collection of high-level mathematical functions to operate on these arrays\nimport numpy as np\n# Matplotlib is a plotting library for python and pyplot gives us a MatLab like plotting framework. We will use this in our plotter function to plot data.\nimport matplotlib.pyplot as plt\n#Seaborn is a Python data visualization library based on matplotlib. It provides a high-level interface for drawing attractive and informative statistical graphics\nimport seaborn as sns\n# Preprocessing allows us to standarsize our data\nfrom sklearn import preprocessing\n# Allows us to split our data into training and testing data\nfrom sklearn.model_selection import train_test_split\n# Allows us to test parameters of classification algorithms and find the best one\nfrom sklearn.model_selection import GridSearchCV\n# Logistic Regression classification algorithm\nfrom sklearn.linear_model import LogisticRegression\n# Support Vector Machine classification algorithm\nfrom sklearn.svm import SVC\n# Decision Tree classification algorithm\nfrom sklearn.tree import DecisionTreeClassifier\n# K Nearest Neighbors classification algorithm\nfrom sklearn.neighbors import KNeighborsClassifier"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "This function is to plot the confusion matrix.\n"
        },
        {
            "cell_type": "code",
            "execution_count": 2,
            "metadata": {},
            "outputs": [],
            "source": "def plot_confusion_matrix(y,y_predict):\n    \"this function plots the confusion matrix\"\n    from sklearn.metrics import confusion_matrix\n\n    cm = confusion_matrix(y, y_predict)\n    ax= plt.subplot()\n    sns.heatmap(cm, annot=True, ax = ax); #annot=True to annotate cells\n    ax.set_xlabel('Predicted labels')\n    ax.set_ylabel('True labels')\n    ax.set_title('Confusion Matrix'); \n    ax.xaxis.set_ticklabels(['did not land', 'land']); ax.yaxis.set_ticklabels(['did not land', 'landed'])"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## Load the dataframe\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Load the data\n"
        },
        {
            "cell_type": "code",
            "execution_count": 3,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/html": "<div>\n<style scoped>\n    .dataframe tbody tr th:only-of-type {\n        vertical-align: middle;\n    }\n\n    .dataframe tbody tr th {\n        vertical-align: top;\n    }\n\n    .dataframe thead th {\n        text-align: right;\n    }\n</style>\n<table border=\"1\" class=\"dataframe\">\n  <thead>\n    <tr style=\"text-align: right;\">\n      <th></th>\n      <th>FlightNumber</th>\n      <th>Date</th>\n      <th>BoosterVersion</th>\n      <th>PayloadMass</th>\n      <th>Orbit</th>\n      <th>LaunchSite</th>\n      <th>Outcome</th>\n      <th>Flights</th>\n      <th>GridFins</th>\n      <th>Reused</th>\n      <th>Legs</th>\n      <th>LandingPad</th>\n      <th>Block</th>\n      <th>ReusedCount</th>\n      <th>Serial</th>\n      <th>Longitude</th>\n      <th>Latitude</th>\n      <th>Class</th>\n    </tr>\n  </thead>\n  <tbody>\n    <tr>\n      <th>0</th>\n      <td>1</td>\n      <td>2010-06-04</td>\n      <td>Falcon 9</td>\n      <td>6104.959412</td>\n      <td>LEO</td>\n      <td>CCAFS SLC 40</td>\n      <td>None None</td>\n      <td>1</td>\n      <td>False</td>\n      <td>False</td>\n      <td>False</td>\n      <td>NaN</td>\n      <td>1.0</td>\n      <td>0</td>\n      <td>B0003</td>\n      <td>-80.577366</td>\n      <td>28.561857</td>\n      <td>0</td>\n    </tr>\n    <tr>\n      <th>1</th>\n      <td>2</td>\n      <td>2012-05-22</td>\n      <td>Falcon 9</td>\n      <td>525.000000</td>\n      <td>LEO</td>\n      <td>CCAFS SLC 40</td>\n      <td>None None</td>\n      <td>1</td>\n      <td>False</td>\n      <td>False</td>\n      <td>False</td>\n      <td>NaN</td>\n      <td>1.0</td>\n      <td>0</td>\n      <td>B0005</td>\n      <td>-80.577366</td>\n      <td>28.561857</td>\n      <td>0</td>\n    </tr>\n    <tr>\n      <th>2</th>\n      <td>3</td>\n      <td>2013-03-01</td>\n      <td>Falcon 9</td>\n      <td>677.000000</td>\n      <td>ISS</td>\n      <td>CCAFS SLC 40</td>\n      <td>None None</td>\n      <td>1</td>\n      <td>False</td>\n      <td>False</td>\n      <td>False</td>\n      <td>NaN</td>\n      <td>1.0</td>\n      <td>0</td>\n      <td>B0007</td>\n      <td>-80.577366</td>\n      <td>28.561857</td>\n      <td>0</td>\n    </tr>\n    <tr>\n      <th>3</th>\n      <td>4</td>\n      <td>2013-09-29</td>\n      <td>Falcon 9</td>\n      <td>500.000000</td>\n      <td>PO</td>\n      <td>VAFB SLC 4E</td>\n      <td>False Ocean</td>\n      <td>1</td>\n      <td>False</td>\n      <td>False</td>\n      <td>False</td>\n      <td>NaN</td>\n      <td>1.0</td>\n      <td>0</td>\n      <td>B1003</td>\n      <td>-120.610829</td>\n      <td>34.632093</td>\n      <td>0</td>\n    </tr>\n    <tr>\n      <th>4</th>\n      <td>5</td>\n      <td>2013-12-03</td>\n      <td>Falcon 9</td>\n      <td>3170.000000</td>\n      <td>GTO</td>\n      <td>CCAFS SLC 40</td>\n      <td>None None</td>\n      <td>1</td>\n      <td>False</td>\n      <td>False</td>\n      <td>False</td>\n      <td>NaN</td>\n      <td>1.0</td>\n      <td>0</td>\n      <td>B1004</td>\n      <td>-80.577366</td>\n      <td>28.561857</td>\n      <td>0</td>\n    </tr>\n  </tbody>\n</table>\n</div>",
                        "text/plain": "   FlightNumber        Date BoosterVersion  PayloadMass Orbit    LaunchSite  \\\n0             1  2010-06-04       Falcon 9  6104.959412   LEO  CCAFS SLC 40   \n1             2  2012-05-22       Falcon 9   525.000000   LEO  CCAFS SLC 40   \n2             3  2013-03-01       Falcon 9   677.000000   ISS  CCAFS SLC 40   \n3             4  2013-09-29       Falcon 9   500.000000    PO   VAFB SLC 4E   \n4             5  2013-12-03       Falcon 9  3170.000000   GTO  CCAFS SLC 40   \n\n       Outcome  Flights  GridFins  Reused   Legs LandingPad  Block  \\\n0    None None        1     False   False  False        NaN    1.0   \n1    None None        1     False   False  False        NaN    1.0   \n2    None None        1     False   False  False        NaN    1.0   \n3  False Ocean        1     False   False  False        NaN    1.0   \n4    None None        1     False   False  False        NaN    1.0   \n\n   ReusedCount Serial   Longitude   Latitude  Class  \n0            0  B0003  -80.577366  28.561857      0  \n1            0  B0005  -80.577366  28.561857      0  \n2            0  B0007  -80.577366  28.561857      0  \n3            0  B1003 -120.610829  34.632093      0  \n4            0  B1004  -80.577366  28.561857      0  "
                    },
                    "execution_count": 3,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "data = pd.read_csv(\"https://cf-courses-data.s3.us.cloud-object-storage.appdomain.cloud/IBM-DS0321EN-SkillsNetwork/datasets/dataset_part_2.csv\")\n\n# If you were unable to complete the previous lab correctly you can uncomment and load this csv\n\n# data = pd.read_csv('https://cf-courses-data.s3.us.cloud-object-storage.appdomain.cloud/IBMDeveloperSkillsNetwork-DS0701EN-SkillsNetwork/api/dataset_part_2.csv')\n\ndata.head()"
        },
        {
            "cell_type": "code",
            "execution_count": 4,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/html": "<div>\n<style scoped>\n    .dataframe tbody tr th:only-of-type {\n        vertical-align: middle;\n    }\n\n    .dataframe tbody tr th {\n        vertical-align: top;\n    }\n\n    .dataframe thead th {\n        text-align: right;\n    }\n</style>\n<table border=\"1\" class=\"dataframe\">\n  <thead>\n    <tr style=\"text-align: right;\">\n      <th></th>\n      <th>FlightNumber</th>\n      <th>PayloadMass</th>\n      <th>Flights</th>\n      <th>Block</th>\n      <th>ReusedCount</th>\n      <th>Orbit_ES-L1</th>\n      <th>Orbit_GEO</th>\n      <th>Orbit_GTO</th>\n      <th>Orbit_HEO</th>\n      <th>Orbit_ISS</th>\n      <th>...</th>\n      <th>Serial_B1058</th>\n      <th>Serial_B1059</th>\n      <th>Serial_B1060</th>\n      <th>Serial_B1062</th>\n      <th>GridFins_False</th>\n      <th>GridFins_True</th>\n      <th>Reused_False</th>\n      <th>Reused_True</th>\n      <th>Legs_False</th>\n      <th>Legs_True</th>\n    </tr>\n  </thead>\n  <tbody>\n    <tr>\n      <th>0</th>\n      <td>1.0</td>\n      <td>6104.959412</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>1</th>\n      <td>2.0</td>\n      <td>525.000000</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>2</th>\n      <td>3.0</td>\n      <td>677.000000</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>3</th>\n      <td>4.0</td>\n      <td>500.000000</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>4</th>\n      <td>5.0</td>\n      <td>3170.000000</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>...</th>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n    </tr>\n    <tr>\n      <th>85</th>\n      <td>86.0</td>\n      <td>15400.000000</td>\n      <td>2.0</td>\n      <td>5.0</td>\n      <td>2.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n    <tr>\n      <th>86</th>\n      <td>87.0</td>\n      <td>15400.000000</td>\n      <td>3.0</td>\n      <td>5.0</td>\n      <td>2.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n    <tr>\n      <th>87</th>\n      <td>88.0</td>\n      <td>15400.000000</td>\n      <td>6.0</td>\n      <td>5.0</td>\n      <td>5.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n    <tr>\n      <th>88</th>\n      <td>89.0</td>\n      <td>15400.000000</td>\n      <td>3.0</td>\n      <td>5.0</td>\n      <td>2.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n    <tr>\n      <th>89</th>\n      <td>90.0</td>\n      <td>3681.000000</td>\n      <td>1.0</td>\n      <td>5.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n  </tbody>\n</table>\n<p>90 rows \u00d7 83 columns</p>\n</div>",
                        "text/plain": "    FlightNumber   PayloadMass  Flights  Block  ReusedCount  Orbit_ES-L1  \\\n0            1.0   6104.959412      1.0    1.0          0.0          0.0   \n1            2.0    525.000000      1.0    1.0          0.0          0.0   \n2            3.0    677.000000      1.0    1.0          0.0          0.0   \n3            4.0    500.000000      1.0    1.0          0.0          0.0   \n4            5.0   3170.000000      1.0    1.0          0.0          0.0   \n..           ...           ...      ...    ...          ...          ...   \n85          86.0  15400.000000      2.0    5.0          2.0          0.0   \n86          87.0  15400.000000      3.0    5.0          2.0          0.0   \n87          88.0  15400.000000      6.0    5.0          5.0          0.0   \n88          89.0  15400.000000      3.0    5.0          2.0          0.0   \n89          90.0   3681.000000      1.0    5.0          0.0          0.0   \n\n    Orbit_GEO  Orbit_GTO  Orbit_HEO  Orbit_ISS  ...  Serial_B1058  \\\n0         0.0        0.0        0.0        0.0  ...           0.0   \n1         0.0        0.0        0.0        0.0  ...           0.0   \n2         0.0        0.0        0.0        1.0  ...           0.0   \n3         0.0        0.0        0.0        0.0  ...           0.0   \n4         0.0        1.0        0.0        0.0  ...           0.0   \n..        ...        ...        ...        ...  ...           ...   \n85        0.0        0.0        0.0        0.0  ...           0.0   \n86        0.0        0.0        0.0        0.0  ...           1.0   \n87        0.0        0.0        0.0        0.0  ...           0.0   \n88        0.0        0.0        0.0        0.0  ...           0.0   \n89        0.0        0.0        0.0        0.0  ...           0.0   \n\n    Serial_B1059  Serial_B1060  Serial_B1062  GridFins_False  GridFins_True  \\\n0            0.0           0.0           0.0             1.0            0.0   \n1            0.0           0.0           0.0             1.0            0.0   \n2            0.0           0.0           0.0             1.0            0.0   \n3            0.0           0.0           0.0             1.0            0.0   \n4            0.0           0.0           0.0             1.0            0.0   \n..           ...           ...           ...             ...            ...   \n85           0.0           1.0           0.0             0.0            1.0   \n86           0.0           0.0           0.0             0.0            1.0   \n87           0.0           0.0           0.0             0.0            1.0   \n88           0.0           1.0           0.0             0.0            1.0   \n89           0.0           0.0           1.0             0.0            1.0   \n\n    Reused_False  Reused_True  Legs_False  Legs_True  \n0            1.0          0.0         1.0        0.0  \n1            1.0          0.0         1.0        0.0  \n2            1.0          0.0         1.0        0.0  \n3            1.0          0.0         1.0        0.0  \n4            1.0          0.0         1.0        0.0  \n..           ...          ...         ...        ...  \n85           0.0          1.0         0.0        1.0  \n86           0.0          1.0         0.0        1.0  \n87           0.0          1.0         0.0        1.0  \n88           0.0          1.0         0.0        1.0  \n89           1.0          0.0         0.0        1.0  \n\n[90 rows x 83 columns]"
                    },
                    "execution_count": 4,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "X = pd.read_csv('https://cf-courses-data.s3.us.cloud-object-storage.appdomain.cloud/IBM-DS0321EN-SkillsNetwork/datasets/dataset_part_3.csv')\n\n# If you were unable to complete the previous lab correctly you can uncomment and load this csv\n\n# X = pd.read_csv('https://cf-courses-data.s3.us.cloud-object-storage.appdomain.cloud/IBMDeveloperSkillsNetwork-DS0701EN-SkillsNetwork/api/dataset_part_3.csv')\n\nX.head(100)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  1\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Create a NumPy array from the column <code>Class</code> in <code>data</code>, by applying the method <code>to_numpy()</code>  then\nassign it  to the variable <code>Y</code>,make sure the output is a  Pandas series (only one bracket df\\['name of  column']).\n"
        },
        {
            "cell_type": "code",
            "execution_count": 5,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "array([0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1,\n       1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,\n       1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1,\n       1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,\n       1, 1])"
                    },
                    "execution_count": 5,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "Y = data.Class.to_numpy()\nY"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  2\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Standardize the data in <code>X</code> then reassign it to the variable  <code>X</code> using the transform provided below.\n"
        },
        {
            "cell_type": "code",
            "execution_count": 6,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/html": "<div>\n<style scoped>\n    .dataframe tbody tr th:only-of-type {\n        vertical-align: middle;\n    }\n\n    .dataframe tbody tr th {\n        vertical-align: top;\n    }\n\n    .dataframe thead th {\n        text-align: right;\n    }\n</style>\n<table border=\"1\" class=\"dataframe\">\n  <thead>\n    <tr style=\"text-align: right;\">\n      <th></th>\n      <th>FlightNumber</th>\n      <th>PayloadMass</th>\n      <th>Flights</th>\n      <th>Block</th>\n      <th>ReusedCount</th>\n      <th>Orbit_ES-L1</th>\n      <th>Orbit_GEO</th>\n      <th>Orbit_GTO</th>\n      <th>Orbit_HEO</th>\n      <th>Orbit_ISS</th>\n      <th>...</th>\n      <th>Serial_B1058</th>\n      <th>Serial_B1059</th>\n      <th>Serial_B1060</th>\n      <th>Serial_B1062</th>\n      <th>GridFins_False</th>\n      <th>GridFins_True</th>\n      <th>Reused_False</th>\n      <th>Reused_True</th>\n      <th>Legs_False</th>\n      <th>Legs_True</th>\n    </tr>\n  </thead>\n  <tbody>\n    <tr>\n      <th>0</th>\n      <td>1.0</td>\n      <td>6104.959412</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>1</th>\n      <td>2.0</td>\n      <td>525.000000</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>2</th>\n      <td>3.0</td>\n      <td>677.000000</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>3</th>\n      <td>4.0</td>\n      <td>500.000000</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>4</th>\n      <td>5.0</td>\n      <td>3170.000000</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n    </tr>\n    <tr>\n      <th>...</th>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n      <td>...</td>\n    </tr>\n    <tr>\n      <th>85</th>\n      <td>86.0</td>\n      <td>15400.000000</td>\n      <td>2.0</td>\n      <td>5.0</td>\n      <td>2.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n    <tr>\n      <th>86</th>\n      <td>87.0</td>\n      <td>15400.000000</td>\n      <td>3.0</td>\n      <td>5.0</td>\n      <td>2.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n    <tr>\n      <th>87</th>\n      <td>88.0</td>\n      <td>15400.000000</td>\n      <td>6.0</td>\n      <td>5.0</td>\n      <td>5.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n    <tr>\n      <th>88</th>\n      <td>89.0</td>\n      <td>15400.000000</td>\n      <td>3.0</td>\n      <td>5.0</td>\n      <td>2.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n    <tr>\n      <th>89</th>\n      <td>90.0</td>\n      <td>3681.000000</td>\n      <td>1.0</td>\n      <td>5.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>...</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n      <td>1.0</td>\n      <td>0.0</td>\n      <td>0.0</td>\n      <td>1.0</td>\n    </tr>\n  </tbody>\n</table>\n<p>90 rows \u00d7 83 columns</p>\n</div>",
                        "text/plain": "    FlightNumber   PayloadMass  Flights  Block  ReusedCount  Orbit_ES-L1  \\\n0            1.0   6104.959412      1.0    1.0          0.0          0.0   \n1            2.0    525.000000      1.0    1.0          0.0          0.0   \n2            3.0    677.000000      1.0    1.0          0.0          0.0   \n3            4.0    500.000000      1.0    1.0          0.0          0.0   \n4            5.0   3170.000000      1.0    1.0          0.0          0.0   \n..           ...           ...      ...    ...          ...          ...   \n85          86.0  15400.000000      2.0    5.0          2.0          0.0   \n86          87.0  15400.000000      3.0    5.0          2.0          0.0   \n87          88.0  15400.000000      6.0    5.0          5.0          0.0   \n88          89.0  15400.000000      3.0    5.0          2.0          0.0   \n89          90.0   3681.000000      1.0    5.0          0.0          0.0   \n\n    Orbit_GEO  Orbit_GTO  Orbit_HEO  Orbit_ISS  ...  Serial_B1058  \\\n0         0.0        0.0        0.0        0.0  ...           0.0   \n1         0.0        0.0        0.0        0.0  ...           0.0   \n2         0.0        0.0        0.0        1.0  ...           0.0   \n3         0.0        0.0        0.0        0.0  ...           0.0   \n4         0.0        1.0        0.0        0.0  ...           0.0   \n..        ...        ...        ...        ...  ...           ...   \n85        0.0        0.0        0.0        0.0  ...           0.0   \n86        0.0        0.0        0.0        0.0  ...           1.0   \n87        0.0        0.0        0.0        0.0  ...           0.0   \n88        0.0        0.0        0.0        0.0  ...           0.0   \n89        0.0        0.0        0.0        0.0  ...           0.0   \n\n    Serial_B1059  Serial_B1060  Serial_B1062  GridFins_False  GridFins_True  \\\n0            0.0           0.0           0.0             1.0            0.0   \n1            0.0           0.0           0.0             1.0            0.0   \n2            0.0           0.0           0.0             1.0            0.0   \n3            0.0           0.0           0.0             1.0            0.0   \n4            0.0           0.0           0.0             1.0            0.0   \n..           ...           ...           ...             ...            ...   \n85           0.0           1.0           0.0             0.0            1.0   \n86           0.0           0.0           0.0             0.0            1.0   \n87           0.0           0.0           0.0             0.0            1.0   \n88           0.0           1.0           0.0             0.0            1.0   \n89           0.0           0.0           1.0             0.0            1.0   \n\n    Reused_False  Reused_True  Legs_False  Legs_True  \n0            1.0          0.0         1.0        0.0  \n1            1.0          0.0         1.0        0.0  \n2            1.0          0.0         1.0        0.0  \n3            1.0          0.0         1.0        0.0  \n4            1.0          0.0         1.0        0.0  \n..           ...          ...         ...        ...  \n85           0.0          1.0         0.0        1.0  \n86           0.0          1.0         0.0        1.0  \n87           0.0          1.0         0.0        1.0  \n88           0.0          1.0         0.0        1.0  \n89           1.0          0.0         0.0        1.0  \n\n[90 rows x 83 columns]"
                    },
                    "execution_count": 6,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "X"
        },
        {
            "cell_type": "code",
            "execution_count": 7,
            "metadata": {},
            "outputs": [],
            "source": "# students get this \ntransform = preprocessing.StandardScaler()"
        },
        {
            "cell_type": "code",
            "execution_count": 8,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "array([[-1.71291154e+00, -1.94814463e-16, -6.53912840e-01, ...,\n        -8.35531692e-01,  1.93309133e+00, -1.93309133e+00],\n       [-1.67441914e+00, -1.19523159e+00, -6.53912840e-01, ...,\n        -8.35531692e-01,  1.93309133e+00, -1.93309133e+00],\n       [-1.63592675e+00, -1.16267307e+00, -6.53912840e-01, ...,\n        -8.35531692e-01,  1.93309133e+00, -1.93309133e+00],\n       ...,\n       [ 1.63592675e+00,  1.99100483e+00,  3.49060516e+00, ...,\n         1.19684269e+00, -5.17306132e-01,  5.17306132e-01],\n       [ 1.67441914e+00,  1.99100483e+00,  1.00389436e+00, ...,\n         1.19684269e+00, -5.17306132e-01,  5.17306132e-01],\n       [ 1.71291154e+00, -5.19213966e-01, -6.53912840e-01, ...,\n        -8.35531692e-01, -5.17306132e-01,  5.17306132e-01]])"
                    },
                    "execution_count": 8,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "X = transform.fit_transform(X)\nX"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "We split the data into training and testing data using the  function  <code>train_test_split</code>.   The training data is divided into validation data, a second set used for training  data; then the models are trained and hyperparameters are selected using the function <code>GridSearchCV</code>.\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  3\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Use the function train_test_split to split the data X and Y into training and test data. Set the parameter test_size to  0.2 and random_state to 2. The training data and test data should be assigned to the following labels.\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "<code>X_train, X_test, Y_train, Y_test</code>\n"
        },
        {
            "cell_type": "code",
            "execution_count": 9,
            "metadata": {},
            "outputs": [],
            "source": "X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2, random_state=2)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "we can see we only have 18 test samples.\n"
        },
        {
            "cell_type": "code",
            "execution_count": 10,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "(18,)"
                    },
                    "execution_count": 10,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "Y_test.shape"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  4\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Create a logistic regression object  then create a  GridSearchCV object  <code>logreg_cv</code> with cv = 10.  Fit the object to find the best parameters from the dictionary <code>parameters</code>.\n"
        },
        {
            "cell_type": "code",
            "execution_count": 11,
            "metadata": {},
            "outputs": [],
            "source": "parameters ={'C':[0.01,0.1,1],\n             'penalty':['l2'],\n             'solver':['lbfgs']}"
        },
        {
            "cell_type": "code",
            "execution_count": 12,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "GridSearchCV(cv=10, estimator=LogisticRegression(random_state=2),\n             param_grid={'C': [0.01, 0.1, 1], 'penalty': ['l2'],\n                         'solver': ['lbfgs']})"
                    },
                    "execution_count": 12,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "parameters ={\"C\":[0.01,0.1,1],'penalty':['l2'], 'solver':['lbfgs']}# l1 lasso l2 ridge\nlr=LogisticRegression(random_state=2)\nlogreg_cv = GridSearchCV(lr, parameters,cv = 10)\nlogreg_cv.fit(X_train,Y_train)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "We output the <code>GridSearchCV</code> object for logistic regression. We display the best parameters using the data attribute <code>best_params\\_</code> and the accuracy on the validation data using the data attribute <code>best_score\\_</code>.\n"
        },
        {
            "cell_type": "code",
            "execution_count": 13,
            "metadata": {},
            "outputs": [
                {
                    "name": "stdout",
                    "output_type": "stream",
                    "text": "tuned hpyerparameters :(best parameters)  {'C': 0.01, 'penalty': 'l2', 'solver': 'lbfgs'}\naccuracy : 0.8464285714285713\n"
                }
            ],
            "source": "print(\"tuned hpyerparameters :(best parameters) \",logreg_cv.best_params_)\nprint(\"accuracy :\",logreg_cv.best_score_)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  5\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Calculate the accuracy on the test data using the method <code>score</code>:\n"
        },
        {
            "cell_type": "code",
            "execution_count": 14,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "0.8333333333333334"
                    },
                    "execution_count": 14,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "lr_final_score = logreg_cv.score(X_test,Y_test)\nlr_final_score"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Lets look at the confusion matrix:\n"
        },
        {
            "cell_type": "code",
            "execution_count": 15,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "image/png": "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\n",
                        "text/plain": "<Figure size 432x288 with 2 Axes>"
                    },
                    "metadata": {
                        "needs_background": "light"
                    },
                    "output_type": "display_data"
                }
            ],
            "source": "yhat=logreg_cv.predict(X_test)\nplot_confusion_matrix(Y_test,yhat)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Examining the confusion matrix, we see that logistic regression can distinguish between the different classes.  We see that the major problem is false positives.\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  6\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Create a support vector machine object then  create a  <code>GridSearchCV</code> object  <code>svm_cv</code> with cv - 10.  Fit the object to find the best parameters from the dictionary <code>parameters</code>.\n"
        },
        {
            "cell_type": "code",
            "execution_count": 16,
            "metadata": {},
            "outputs": [],
            "source": "parameters = {'kernel':('linear', 'rbf','poly','rbf', 'sigmoid'),\n              'C': np.logspace(-3, 3, 5),\n              'gamma':np.logspace(-3, 3, 5)}\nsvm = SVC(random_state=2)"
        },
        {
            "cell_type": "code",
            "execution_count": 17,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "GridSearchCV(cv=10, estimator=SVC(random_state=2),\n             param_grid={'C': array([1.00000000e-03, 3.16227766e-02, 1.00000000e+00, 3.16227766e+01,\n       1.00000000e+03]),\n                         'gamma': array([1.00000000e-03, 3.16227766e-02, 1.00000000e+00, 3.16227766e+01,\n       1.00000000e+03]),\n                         'kernel': ('linear', 'rbf', 'poly', 'rbf', 'sigmoid')})"
                    },
                    "execution_count": 17,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "svm_cv = GridSearchCV(svm, parameters,cv = 10)\nsvm_cv.fit(X_train,Y_train)"
        },
        {
            "cell_type": "code",
            "execution_count": 18,
            "metadata": {},
            "outputs": [
                {
                    "name": "stdout",
                    "output_type": "stream",
                    "text": "tuned hpyerparameters :(best parameters)  {'C': 1.0, 'gamma': 0.03162277660168379, 'kernel': 'sigmoid'}\naccuracy : 0.8482142857142856\n"
                }
            ],
            "source": "print(\"tuned hpyerparameters :(best parameters) \",svm_cv.best_params_)\nprint(\"accuracy :\",svm_cv.best_score_)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  7\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Calculate the accuracy on the test data using the method <code>score</code>:\n"
        },
        {
            "cell_type": "code",
            "execution_count": 19,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "0.8333333333333334"
                    },
                    "execution_count": 19,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "svm_final_score = svm_cv.score(X_test,Y_test)\nsvm_final_score"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "We can plot the confusion matrix\n"
        },
        {
            "cell_type": "code",
            "execution_count": 20,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "image/png": "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\n",
                        "text/plain": "<Figure size 432x288 with 2 Axes>"
                    },
                    "metadata": {
                        "needs_background": "light"
                    },
                    "output_type": "display_data"
                }
            ],
            "source": "yhat=svm_cv.predict(X_test)\nplot_confusion_matrix(Y_test,yhat)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  8\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Create a decision tree classifier object then  create a  <code>GridSearchCV</code> object  <code>tree_cv</code> with cv = 10.  Fit the object to find the best parameters from the dictionary <code>parameters</code>.\n"
        },
        {
            "cell_type": "code",
            "execution_count": 21,
            "metadata": {},
            "outputs": [],
            "source": "parameters = {'criterion': ['gini', 'entropy'],\n     'splitter': ['best', 'random'],\n     'max_depth': [2*n for n in range(1,10)],\n     'max_features': ['auto', 'sqrt'],\n     'min_samples_leaf': [1, 2, 4],\n     'min_samples_split': [2, 5, 10]}\n\ntree = DecisionTreeClassifier(random_state=2)"
        },
        {
            "cell_type": "code",
            "execution_count": 22,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "GridSearchCV(cv=10, estimator=DecisionTreeClassifier(random_state=2),\n             param_grid={'criterion': ['gini', 'entropy'],\n                         'max_depth': [2, 4, 6, 8, 10, 12, 14, 16, 18],\n                         'max_features': ['auto', 'sqrt'],\n                         'min_samples_leaf': [1, 2, 4],\n                         'min_samples_split': [2, 5, 10],\n                         'splitter': ['best', 'random']})"
                    },
                    "execution_count": 22,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "tree_cv = GridSearchCV(tree, parameters,cv = 10)\ntree_cv.fit(X_train,Y_train)"
        },
        {
            "cell_type": "code",
            "execution_count": 23,
            "metadata": {},
            "outputs": [
                {
                    "name": "stdout",
                    "output_type": "stream",
                    "text": "tuned hpyerparameters :(best parameters)  {'criterion': 'gini', 'max_depth': 8, 'max_features': 'auto', 'min_samples_leaf': 2, 'min_samples_split': 10, 'splitter': 'best'}\naccuracy : 0.8767857142857143\n"
                }
            ],
            "source": "print(\"tuned hpyerparameters :(best parameters) \",tree_cv.best_params_)\nprint(\"accuracy :\",tree_cv.best_score_)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  9\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Calculate the accuracy of tree_cv on the test data using the method <code>score</code>:\n"
        },
        {
            "cell_type": "code",
            "execution_count": 24,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "0.8333333333333334"
                    },
                    "execution_count": 24,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "tree_final_score = tree_cv.score(X_test,Y_test)\ntree_final_score"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "We can plot the confusion matrix\n"
        },
        {
            "cell_type": "code",
            "execution_count": 25,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "image/png": "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\n",
                        "text/plain": "<Figure size 432x288 with 2 Axes>"
                    },
                    "metadata": {
                        "needs_background": "light"
                    },
                    "output_type": "display_data"
                }
            ],
            "source": "yhat = tree_cv.predict(X_test)\nplot_confusion_matrix(Y_test,yhat)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  10\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Create a k nearest neighbors object then  create a  <code>GridSearchCV</code> object  <code>knn_cv</code> with cv = 10.  Fit the object to find the best parameters from the dictionary <code>parameters</code>.\n"
        },
        {
            "cell_type": "code",
            "execution_count": 26,
            "metadata": {},
            "outputs": [],
            "source": "parameters = {'n_neighbors': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],\n              'algorithm': ['auto', 'ball_tree', 'kd_tree', 'brute'],\n              'p': [1,2]}\n\nKNN = KNeighborsClassifier()"
        },
        {
            "cell_type": "code",
            "execution_count": 27,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "GridSearchCV(cv=10, estimator=KNeighborsClassifier(),\n             param_grid={'algorithm': ['auto', 'ball_tree', 'kd_tree', 'brute'],\n                         'n_neighbors': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],\n                         'p': [1, 2]})"
                    },
                    "execution_count": 27,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "knn_cv = GridSearchCV(KNN, parameters,cv = 10)\nknn_cv.fit(X_train,Y_train)"
        },
        {
            "cell_type": "code",
            "execution_count": 28,
            "metadata": {},
            "outputs": [
                {
                    "name": "stdout",
                    "output_type": "stream",
                    "text": "tuned hpyerparameters :(best parameters)  {'algorithm': 'auto', 'n_neighbors': 10, 'p': 1}\naccuracy : 0.8482142857142858\n"
                }
            ],
            "source": "print(\"tuned hpyerparameters :(best parameters) \",knn_cv.best_params_)\nprint(\"accuracy :\",knn_cv.best_score_)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  11\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Calculate the accuracy of tree_cv on the test data using the method <code>score</code>:\n"
        },
        {
            "cell_type": "code",
            "execution_count": 29,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "text/plain": "0.8333333333333334"
                    },
                    "execution_count": 29,
                    "metadata": {},
                    "output_type": "execute_result"
                }
            ],
            "source": "knn_final_score = knn_cv.score(X_test,Y_test)\nknn_final_score"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "We can plot the confusion matrix\n"
        },
        {
            "cell_type": "code",
            "execution_count": 30,
            "metadata": {},
            "outputs": [
                {
                    "data": {
                        "image/png": "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\n",
                        "text/plain": "<Figure size 432x288 with 2 Axes>"
                    },
                    "metadata": {
                        "needs_background": "light"
                    },
                    "output_type": "display_data"
                }
            ],
            "source": "yhat = knn_cv.predict(X_test)\nplot_confusion_matrix(Y_test,yhat)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## TASK  12\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Find the method performs best:\n"
        },
        {
            "cell_type": "code",
            "execution_count": 31,
            "metadata": {},
            "outputs": [
                {
                    "name": "stdout",
                    "output_type": "stream",
                    "text": "lr_final_score: 0.8333333333333334 \nsvm_final_score: 0.8333333333333334 \ntree_final_score: 0.8333333333333334 \nknn_final_score: 0.8333333333333334\n"
                }
            ],
            "source": "print('lr_final_score:',lr_final_score, '\\nsvm_final_score:',svm_final_score, '\\ntree_final_score:',tree_final_score, '\\nknn_final_score:',knn_final_score)"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## Authors\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "<a href=\"https://www.linkedin.com/in/joseph-s-50398b136/?utm_medium=Exinfluencer&utm_source=Exinfluencer&utm_content=000026UJ&utm_term=10006555&utm_id=NA-SkillsNetwork-Channel-SkillsNetworkCoursesIBMDS0321ENSkillsNetwork26802033-2021-01-01\">Joseph Santarcangelo</a> has a PhD in Electrical Engineering, his research focused on using machine learning, signal processing, and computer vision to determine how videos impact human cognition. Joseph has been working for IBM since he completed his PhD.\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "## Change Log\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "| Date (YYYY-MM-DD) | Version | Changed By    | Change Description      |\n| ----------------- | ------- | ------------- | ----------------------- |\n| 2021-08-31        | 1.1     | Lakshmi Holla | Modified markdown       |\n| 2020-09-20        | 1.0     | Joseph        | Modified Multiple Areas |\n"
        },
        {
            "cell_type": "markdown",
            "metadata": {},
            "source": "Copyright \u00a9 2020 IBM Corporation. All rights reserved.\n"
        }
    ],
    "metadata": {
        "kernelspec": {
            "display_name": "Python 3.8",
            "language": "python",
            "name": "python3"
        },
        "language_info": {
            "codemirror_mode": {
                "name": "ipython",
                "version": 3
            },
            "file_extension": ".py",
            "mimetype": "text/x-python",
            "name": "python",
            "nbconvert_exporter": "python",
            "pygments_lexer": "ipython3",
            "version": "3.8.12"
        }
    },
    "nbformat": 4,
    "nbformat_minor": 4
}
