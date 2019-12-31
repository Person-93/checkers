embed_resources ( image_files BlackKing.png BlackPiece.png RedKing.png RedPiece.png CheckersBoard.jpg )
add_library ( images ${image_files} )

embed_resources ( sound_files knock.wav )
add_library ( sounds ${sound_files} )
