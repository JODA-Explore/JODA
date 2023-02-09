from typing import Union, List, Tuple


def estimate_usage(predicate: str, state: dict, doc_count: int) -> Union[None, int]:
    """
    Estimate the work remaining after the predicate has been evaluated with the index.
    None can be returned if the index is not applicable.
    """
    if predicate in state:
        return 0
    else:
        return None


def execute_state(predicate: str, state) -> Union[None, Tuple[List[bool],bool]]:
    """
    Execute the predicate using only the state.
    None can be returned if this is not possible.
    """
    return (state[predicate], True)


def execute_docs(predicate: str, state, docs) -> Tuple[List[bool],bool]:
    """
    Execute the predicate using the state and a list of all the documents.
    """

    return None

def improve_index(predicate: str, state, doc_index: List[bool]):
    """
    Improve the index using the final result of the query
    """
    state[predicate] = doc_index

def init_index():
    """
    Returns the initial empty state of the index
    """

    return dict()