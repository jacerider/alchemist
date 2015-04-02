( function() {

  'use strict';

  var alchemistToolbar = function() {
    this.toolbar = CKEDITOR.document.getById('alchemist-top');
    this.toolbarDimensions = false;
    this.editorPosition = false;
    this.documentPosition = false;
    this.is_visible = false;
  };

  // Create parent divs.
  var div = document.createElement('div');
  div.id = 'alchemist-top';
  document.body.appendChild(div);
  var div = document.createElement('div');
  div.id = 'alchemist-bottom';
  document.body.appendChild(div);

  var containerTpl = CKEDITOR.addTemplate( 'sharedcontainer', '<div' +
    ' id="cke_{name}"' +
    ' class="cke {id} cke_reset_all cke_chrome cke_editor_{name} cke_shared cke_detached cke_{langDir} ' + CKEDITOR.env.cssClass + '"' +
    ' dir="{langDir}"' +
    ' title="' + ( CKEDITOR.env.gecko ? ' ' : '' ) + '"' +
    ' lang="{langCode}"' +
    ' role="presentation"' +
    '>' +
      '<div class="cke_inner">' +
        '<div id="{spaceId}" class="cke_{space}" role="presentation">{content}</div>' +
      '</div>' +
    '</div>' );

  CKEDITOR.plugins.add( 'alchemistToolbar', {
    init: function( editor ) {

      var spaces = {top: 'alchemist-top',bottom: 'alchemist-bottom'}, mousepos;

      // Create toolbars on #loaded (like themed creator), but do that
      // with higher priority to block the default scenario.
      editor.on( 'loaded', function() {
        if ( spaces ) {
          for ( var spaceName in spaces ){
            create( editor, spaceName, spaces[ spaceName ] );
          }
        }
      }, null, null, 9 );


      editor.on( 'focus', function() {
        editor.alchemistToolbar = new alchemistToolbar();
        editor.alchemistToolbar.toolbar.$.removeAttribute('style');

        // Hide by default
        position(editor, 'hide');
      });


      /**
       * Do the magic: Attach eventhandlers to see if text is selected
       * When text is selected then show the floating toolbar, else hide it
       */
      editor.on('contentDom', function( event ) {

        /**
         * Attach an eventhandler to the mouse-up event
         */
        editor.window.on('mouseup', function( mouse_event ) {
          if(editor.alchemistToolbar) position(editor);
        });

        editor.container.on('keyup', function( mouse_event ) {
          position(editor);
        });

        editor.container.on('focus', function( mouse_event ) {
          position(editor, 'hide');
        });

      });


    }
  });


  function position( editor, op ) {

    // Because we bind mousup to window in order to catch mouse drags outsize
    // of editing area, we only want to resize when current editor = focused editor.
    var ck_instance_name = false;
    for ( var ck_instance in CKEDITOR.instances ){
      if (CKEDITOR.instances[ck_instance].focusManager.hasFocus){
        if(ck_instance != editor.name) return;
      }
    }

    var getElementPosition, getToolbar, setToolbarPosition, getEditorPosition,
      getDocumentPosition, getSelectionPosition, getToolbarDimensions, calculatePosition,
      hideToolbar, showToolbar;

    /**
     * Get position and dimensions of an element.
     */
    getElementPosition = function(element){
      var position = {left:0,top:0,height:0,width:0};
      if(!element){
        return position;
      }
      var obj = element.$;
      position.height = obj.offsetHeight;
      position.width = obj.offsetWidth;

      while(obj) {
        position.left += (obj.offsetLeft - obj.scrollLeft + obj.clientLeft);
        position.top += (obj.offsetTop - obj.scrollTop + obj.clientTop);
        obj = obj.offsetParent;
      }
      return position;
    }

    /**
     * Get current selection dimensions and position.
     */
    getSelectionPosition = function(){
      var element = CKEDITOR.document.getById('alchemist-target');
      return getElementPosition(element);
    }

    /**
     * Get current editor dimensions and position.
     */
    getEditorPosition = function(){
      if (! editor.alchemistToolbar.editorPosition) {
        editor.alchemistToolbar.editorPosition = getElementPosition(editor.container);
      }
      return editor.alchemistToolbar.editorPosition;
    }

    /**
     * Get current window dimensions and position.
     */
    getDocumentPosition = function(){
      if (! editor.alchemistToolbar.documentPosition) {
        editor.alchemistToolbar.documentPosition = {width: document.body.clientWidth, height: document.body.clientHeight};
      }
      return editor.alchemistToolbar.documentPosition;
    }

    /**
     * Returns the size of the floating toolbar
     */
    getToolbarDimensions = function(reset) {
      if (! editor.alchemistToolbar.toolbarDimensions || reset) {
        var obj = getToolbar();
        editor.alchemistToolbar.toolbarDimensions = {
          width: obj.$.offsetWidth,
          height: obj.$.offsetHeight
        };
      }
      return editor.alchemistToolbar.toolbarDimensions;
    }

    getToolbar = function(){
      return editor.alchemistToolbar.toolbar;
    }

    /**
     * Calculates the position for the toolbar
     */
    calculatePosition = function(selectionPosition, toolbarDimensions, editorPosition, documentPosition) {
      var toolpos = {
        x: (selectionPosition.left + (selectionPosition.width / 2)) - (toolbarDimensions.width / 2),
        y: selectionPosition.top - (toolbarDimensions.height + 5),
      }

      // We have more than 1 line selected.
      if(selectionPosition.height > 40){
        toolpos.x = (editorPosition.left + (editorPosition.width / 2)) - (toolbarDimensions.width / 2);
      }

      // make sure toolbar does not extend out of the left CKEditor border
      // if (toolpos.x < editorPosition.left){
      if (toolpos.x < 0){
        toolpos.x = editorPosition.left;
      }

      // make sure toolbar does not extend out of the right CKEditor border
      // if (selectionPosition.left + (toolbarDimensions.width/2) >= editorPosition.left + editorPosition.width ){
      if (selectionPosition.left + (toolbarDimensions.width/2) >= documentPosition.width ){
        toolpos.x = editorPosition.left + editorPosition.width - toolbarDimensions.width;
      }

      return toolpos;
    }

    setToolbarPosition = function(){
      var toolbar = getToolbar();
      var selectionPosition = getSelectionPosition();
      var editorPosition = getEditorPosition();
      var toolbarDimensions = getToolbarDimensions();
      var documentPosition = getDocumentPosition();
      if(toolbarDimensions.width > editorPosition.width){
        toolbar.setStyles({
          'width' : editorPosition.width + 'px'
        });
        toolbarDimensions = getToolbarDimensions(true);
      }
      var position = calculatePosition(selectionPosition, toolbarDimensions, editorPosition, documentPosition);
      toolbar.setStyles({
        'left' : position.x + 'px',
        'top' : position.y + 'px'
      });

    }

    showToolbar = function(){
      var toolbar = getToolbar();
      toolbar.$.className = 'active in';
    }

    hideToolbar = function(classes){
      var toolbar = getToolbar();
      classes = classes ? ' ' + classes : '';
      toolbar.$.className = 'active out' + classes;
    }

    if(op == 'hide'){
      hideToolbar('hide');
      return;
    }

    // Act on text selection.
    setTimeout(function(){
      var selection = editor.getSelection();
      if( selection.getSelectedText() ){

        var range = selection.getRanges(1)[0];

        var element = new CKEDITOR.style({
          element: "span",
          attributes: {id:'alchemist-target'},
          type: CKEDITOR.STYLE_INLINE
        });
        editor.applyStyle( element );

        showToolbar();
        setToolbarPosition();

        setTimeout(function(){
          editor.removeStyle( element );
        },100);
      }
      else{
        hideToolbar();
      }
    },10);

  }


  /**
   * Append toolbars to parent div.
   */
  function create( editor, spaceName, targetId ) {
    var target = CKEDITOR.document.getById( targetId ),
      innerHtml, space;

    if ( target ) {
      // Have other plugins filling the space.
      innerHtml = editor.fire( 'uiSpace', { space: spaceName, html: '' } ).html;

      if ( innerHtml ) {
        // Block the uiSpace handling by others (e.g. themed-ui).
        editor.on( 'uiSpace', function( ev ) {
          if ( ev.data.space == spaceName )
            ev.cancel();
        }, null, null, 1 );  // Hi-priority

        // Inject the space into the target.
        space = target.append( CKEDITOR.dom.element.createFromHtml( containerTpl.output( {
          id: editor.id,
          name: editor.name,
          langDir: editor.lang.dir,
          langCode: editor.langCode,
          space: spaceName,
          spaceId: editor.ui.spaceId( spaceName ),
          content: innerHtml
        } ) ) );

        // Only the first container starts visible. Others get hidden.
        if ( target.getCustomData( 'cke_hasshared' ) )
          space.hide();
        else
          target.setCustomData( 'cke_hasshared', 1 );

        // There's no need for the space to be selectable.
        space.unselectable();

        // Hide initially
        space.hide();

        // Prevent clicking on non-buttons area of the space from blurring editor.
        space.on( 'mousedown', function( evt ) {
          evt = evt.data;
          if ( !evt.getTarget().hasAscendant( 'a', 1 ) )
            evt.preventDefault();
        } );

        // Register this UI space to the focus manager.
        editor.focusManager.add( space, 1 );

        // When the editor gets focus, show the space container, hiding others.
        editor.on( 'focus', function() {
          for ( var i = 0, sibling, children = target.getChildren(); ( sibling = children.getItem( i ) ); i++ ) {
            if ( sibling.type == CKEDITOR.NODE_ELEMENT &&
              !sibling.equals( space ) &&
              sibling.hasClass( 'cke_shared' ) ) {
              sibling.hide();
            }
          }

          space.show();
        });

        editor.on( 'blur', function() {
          space.hide();
        });

        editor.on( 'destroy', function() {
          space.remove();
        });
      }
    }
  }
} )();
