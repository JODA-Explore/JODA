# pip install fasttext
import fasttext

    # for estimating the text language one needs to download the pretrained model
    # curl https://dl.fbaipublicfiles.com/fasttext/supervised-models/lid.176.bin  -o lid.176.bin
PRETRAINED_MODEL_PATH = '/home/nico/projects/JODA/test/modules/value/lid.176.bin'
    # loads the pretrained model
model = fasttext.load_model(PRETRAINED_MODEL_PATH)

def detect_text_language(text, number_of_languages):
    # performs prediction on the given text
    prediction_original = model.predict(text, k=number_of_languages)

    # gather only the language and probability for every prediction as a pair:
    # (language, language_probability)
    predictions = list()
    for pred_id, pred in enumerate(prediction_original[0]):
        predictions.append((pred.split('__label__')[1], prediction_original[1][pred_id]))


    return predictions

# Number of arguments for main function (get_value)
num_args = 1
# Given a number of arguments, returns an arbitrary value
def get_value(arg1):
    if arg1 is None:
        return None
    prediction = detect_text_language(arg1, 1)
    return prediction[0][0]

# if __name__ == '__main__':
#     # the label is according to the ISO 639 standard https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
#     prediction = detect_text_language("здраво како си", 1)

#     print(prediction)


# import time
# import json

# start = time.time()
# with open("/home/nico/json/smalltwitter/Twitter.json") as f:
#     for line in f:
#         j_content = json.loads(line)
#         if "text" in j_content:
#             text = j_content['text']
#             text = text.replace("\n", "")
#             language = get_value(text)
#             print(f'{{\"language\":\"{language}\",\"text\":\"{text}\"\}}')
# end = time.time()
# print(end - start)