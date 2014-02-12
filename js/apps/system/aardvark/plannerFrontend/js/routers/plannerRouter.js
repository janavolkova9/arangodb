/*jslint indent: 2, nomen: true, maxlen: 100, sloppy: true, vars: true, white: true, plusplus: true, newcap: true */
/*global window, $, Backbone, document, arangoCollection,arangoHelper,dashboardView,arangoDatabase*/

(function() {
  "use strict";

  window.PlannerRouter = Backbone.Router.extend({

    routes: {
      "planTest"             : "planTest",
      "planSymmetric"        : "planSymmetric",
      "planAsymmetric"       : "planAsymmetric",
      "planScenarioSelector" : "planScenarioSelector"
    },

    planTest: function() {
      if (!this.planTestView) {
        this.planTestView = new window.PlanTestView();
      }
      this.planTestView.render();
    },

    planSymmetric: function() {
      if (!this.planSymmetricView) {
        this.planSymmetricView = new window.PlanSymmetricView();
      }
      this.planTestView.render();
    },

    planAsymmetric: function() {
      if (!this.planAsymmetricView) {
        this.planAsymmetricView = new window.PlanAsymmetricView();
      }
      this.planAsymmetricView.render();
    },

    planScenarioSelector: function() {
      if (!this.PlanScenarioSelector) {
          this.PlanScenarioSelector = new window.PlanScenarioSelectorView();
      }
      this.PlanScenarioSelector.render();
    },

    initialize: function () {
      /*
      this.footerView = new window.FooterView();
      this.footerView.render();
      */
    },

    handleResize: function() {
     // Not needed here
    }
  });

}());
