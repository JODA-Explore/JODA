import os
import pickle
from sklearn.linear_model import SGDClassifier

batch_size = 1000

# Number of arguments for main function (aggregate)
num_args = 1

def save_mode_as_pickle_file(model, path_to_save):
    pickle.dump(model, open(path_to_save, 'wb'))

def extract_document_features(document, num_features):
    x = []
    for i in range(num_features):
        # extract all the values for the features
        x.append(document['feature_' + str(i)])
    # extract the category
    y = document['category']

    return x, y

def extract_data_to_right_format(documents):
    X = list()
    Y = list()
    for document in documents:
        # extract the needed features for X and Y
        x, y = extract_document_features(document, num_features=10)
        X.append(x)
        Y.append(int(y))

    return X, Y

def train_batch(state):
    # transform the batch documents to features needed for the model

    X_batch, Y_batch = extract_data_to_right_format(state['data'])

    # train the model on the current batch
    state['model'].partial_fit(X_batch, Y_batch, classes=list(range(2)))

    return state


# Aggregate function, takes a state and the specified number of arguments and returns a new state
def aggregate(state, arg1):
    global batch_size

    state['data'].append(arg1)
    if len(state['data']) == batch_size:
        state = train_batch(state)
        state['data'] = list()
    return state

# Merges the own state with another state
def merge(state, other):
    state["data"] += other["data"]
    if len(state["data"]) >= batch_size:
        state = train_batch(state)
        state['data'] = list()
    return state

# Returns the aggregation result based on the state
def finalize(state):
    if len(state['data']) > 0:
        state = train_batch(state)
        # store the model
        save_mode_as_pickle_file(state['model'], 'sgd_classifier_model.pkl')
    return True

# Initial state
def init_state():
    state_dictionary = dict()
    state_dictionary['data'] = list()
    if os.path.exists('sgd_classifier_model.pkl'):
        with open('sgd_classifier_model.pkl', 'rb') as pickle_file:
            state_dictionary['model'] = pickle.load(pickle_file)
    else:
        state_dictionary['model'] = SGDClassifier(random_state=123)

    return state_dictionary

import json
s = init_state()
with open("/home/nico/json/tmp/example_complete_train_json_data.json") as f:
    for line in f:
        j_content = json.loads(line)
        s = aggregate(s,j_content)

v = finalize(s)

# if os.path.exists('sgd_classifier_model.pkl'):
#         with open('sgd_classifier_model.pkl', 'rb') as pickle_file:
#             classifier = pickle.load(pickle_file)

#         Y_train_preds = []
#         # go over the train batches and make predictions
#         for j in range(X_train.shape[0]):
#             Y_preds = classifier.predict(X_train[j])
#             Y_train_preds.extend(Y_preds.tolist())

#         print("Train Accuracy      : {}".format(accuracy_score(Y_train.reshape(-1), Y_train_preds)))

#         Y_test_preds = []
#         # go over the test batches and make predictions
#         for j in range(X_test.shape[0]):
#             Y_preds = classifier.predict(X_test[j])
#             Y_test_preds.extend(Y_preds.tolist())

#         print("Test Accuracy      : {}".format(accuracy_score(Y_test.reshape(-1), Y_test_preds)))