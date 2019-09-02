import React, { useEffect } from "react";
import { Redirect } from "react-router-dom";
import PropTypes from "prop-types";
import { connect } from "react-redux";
import Entries from "./entities/Entries";
import Spinner from "./Spinner";
import ImageEditor from "./entities/ImageEditor";
import AppEditor from "./entities/AppEditor";
import GeomEditor from "./entities/GeomEditor";
import MaterialEditor from "./entities/MaterialEditor";
import { wasmSetCanvasSize } from "../../actions/wasm";
import EntityUpdateContent from "./entities/EntityUpdateContent";
import EntityMetaSection from "./entities/EntityMetaSection";
import RenderParamsToolbar from "./entities/RenderParamsToolbar";
import { getFullEntity } from "../../actions/entities";
import store from "../../store";

const containerClassFromGroup = group => {
  switch (group) {
    case "geom":
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <GeomEditor />
      };
    case "material":
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <MaterialEditor />
      };
    case "image":
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <ImageEditor />
      };
    case "app":
      return {
        mainContainerClass: "AppEditorRenderGrid",
        mainContainerDiv: <AppEditor />
      };
    default:
      return {
        mainContainerClass: "GeomEditorRenderGrid",
        mainContainerDiv: <GeomEditor />
      };
  }
};

const DashboardProject = ({
  currentEntity,
  entities,
  loading,
  group,
  userToken,
  userData
}) => {
  let canvasContainer = React.useRef(null);

  useEffect(() => {
    // Shortcut to go straight to app/coding from the outset for most projects
    if (group === "app" && entities.length === 1 && !currentEntity) {
      store.dispatch(getFullEntity(entities[0]));
    }
  }, [currentEntity, entities, group]);

  if (!userData || !userData.project) {
    return <Redirect to="/" />;
  }

  const { mainContainerClass, mainContainerDiv } = containerClassFromGroup(
    group
  );

  const bShowEntityCanvas = currentEntity && group !== "app";

  if (canvasContainer.current) {
    const rect = canvasContainer.current.getBoundingClientRect();
    store.dispatch(wasmSetCanvasSize(rect));
  }
  // const entityBased = group !== "app";

  const mainEditorDiv = (
    <div className={mainContainerClass}>
      {bShowEntityCanvas && <RenderParamsToolbar />}
      {bShowEntityCanvas && <EntityUpdateContent />}
      <div className="EntryEditorRender" ref={canvasContainer}></div>
      {currentEntity && mainContainerDiv}
      {currentEntity && bShowEntityCanvas && <EntityMetaSection />}
    </div>
  );

  return (
    <div className="dashboardContainer">
      {loading && <Spinner />}
      <Entries cname="thumbs-a thumbsEntityArea" />
      <div className="editor-a">{mainEditorDiv}</div>
    </div>
  );
};

DashboardProject.propTypes = {
  currentEntity: PropTypes.object,
  entities: PropTypes.array,
  loading: PropTypes.bool,
  group: PropTypes.string,
  userToken: PropTypes.string,
  userData: PropTypes.object
};

const mapStateToProps = state => ({
  currentEntity: state.entities.currentEntity,
  entities: state.entities.entries,
  loading: state.entities.loading,
  group: state.entities.group,
  userToken: state.auth.token,
  userData: state.auth.userdata
});

export default connect(
  mapStateToProps,
  {}
)(DashboardProject);
