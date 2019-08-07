import React from "react";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Spinner from "../Spinner";
import {
  updateEntriesPartialSearch,
  getFullEntity
} from "../../../actions/entities";
import EntityDragAndImport from "./EntityDragAndImport";
import EntitiesSearchBox from "./EntitiesSearchBox";
import EntitiesThumbHandler from "./EntitiesThumbHandler";

const Entries = ({ loading, entries, currentEntity, cname }) => {
  return loading ? (
    <Spinner />
  ) : (
    <div className={cname}>
      <EntitiesSearchBox
        updatePartialSearch={updateEntriesPartialSearch}
        placeHolderText="Filter..."
      />
      <EntityDragAndImport />
      <EntitiesThumbHandler
        currentEntity={currentEntity}
        entries={entries}
        onClicked={getFullEntity}
      />
    </div>
  );
};

Entries.propTypes = {
  loading: PropTypes.bool,
  entries: PropTypes.array,
  currentEntity: PropTypes.object
};

const mapStateToProps = state => ({
  loading: state.auth.loading,
  entries: state.entities.entriesFiltered,
  currentEntity: state.entities.currentEntity
});

export default connect(
  mapStateToProps,
  {}
)(Entries);
