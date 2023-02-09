import pickle

# Number of arguments for main function (get_value)
num_args = 1


def extract_document_features(document, num_features):
    x = []
    for i in range(num_features):
        # extract all the values for the features
        x.append(document['feature_' + str(i)])
    # extract the category
    y = document['category']

    return x, y


pickle_file = open('sgd_classifier_model.pkl', 'rb')
# Given a number of arguments, returns an arbitrary value
classifier = pickle.load(pickle_file)

def get_value(arg1):
    features, label = extract_document_features(arg1, 10)

    return [label, classifier.predict([features])[0].item()]
